#include <cmath>
#include <random>

#include "Game.hpp"
#include "menu/Menu.hpp"

Game::Game(const win::Area<float> &area, bool runbot)
	: showmenu(true)
	, runbot(runbot)
	, area(area)
	, match(area)
{
	std::mt19937 mersenne(time(NULL));
	color = runbot ? (Color)std::uniform_int_distribution<int>(0, 5)(mersenne) : Color::red;
	reset();

	if (runbot)
	{
		while (!match.join("::1")) std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void Game::play(SimulationHost &host)
{
	std::vector<char> textinput(20);
	textinput.clear();

	while (!host.quit())
	{
		if (showmenu && !runbot)
		{
			showmenu = false;
			color = Menu::menu_main(host, match, "");

			if (host.quit())
				return;
		}

		auto &renderables = host.get_renderables();
		const auto input = host.get_input();
		host.get_text_input(textinput);

		tick(renderables, input);

		host.release_renderables(renderables);

		host.sleep();
	}
}

void Game::tick(Renderables &renderables, const Input &input)
{
	if (match.hosting())
		match.host_get_data(networkdata.guest_paddle_color, networkdata.guest_paddle_y);
	else
		match.guest_get_data(networkdata.host_paddle_color, networkdata.host_paddle_y, networkdata.ball_x, networkdata.ball_y, networkdata.ball_xv, networkdata.ball_yv, networkdata.host_score, networkdata.guest_score);

	const auto reason = match.errored();
	if (reason != NetworkMatch::ErrorReason::none)
	{
		match.reset();
		showmenu = true;
		return;
	}

	process_ball(renderables.lerped_renderables, renderables.light_renderables);
	renderables.lerped_renderables.emplace_back(process_player_paddle(input));
	renderables.lerped_renderables.emplace_back(process_opponent_paddle());

	char scoretext[10];
	snprintf(scoretext, sizeof(scoretext), "%d", networkdata.guest_score);
	renderables.text_renderables.emplace_back(0, -4.0f, 3.0f, true, TextRenderable::Type::smol, win::Color<float>(0.6f, 0.6f, 0.6f, 1.0f), scoretext);
	snprintf(scoretext, sizeof(scoretext), "%d", networkdata.host_score);
	renderables.text_renderables.emplace_back(0, 4.0f, 3.0f, true, TextRenderable::Type::smol, win::Color<float>(0.6f, 0.6f, 0.6f, 1.0f), scoretext);

	if (match.hosting())
		match.host_send_data(networkdata.host_paddle_color, networkdata.host_paddle_y, networkdata.ball_x, networkdata.ball_y, networkdata.ball_xv, networkdata.ball_yv, networkdata.host_score, networkdata.guest_score);
	else
		match.guest_send_data(networkdata.guest_paddle_color, networkdata.guest_paddle_y);

	const auto reason2 = match.errored();
	if (reason2 != NetworkMatch::ErrorReason::none)
	{
		match.reset();
		showmenu = true;
	}
}

void Game::process_ball(std::vector<LerpedRenderable> &renderables, std::vector<LerpedLightRenderable> &light_renderables)
{
	const float oldx = ball.x;
	const float oldy = ball.y;

	bool bounce = false;

	if (match.hosting())
	{
		const float slowdown = 1.0f;
		ball.x += ball.xv * slowdown;
		ball.y += ball.yv * slowdown;

		// check for collision with paddles
		if (collide(ball, host))
		{
			bounce = true;
			ball.x = (host.x - Ball::width) + Ball::squishiness;
			ball.xv = -ball.xv;
			ball.yv = get_ball_yv(ball, host);
		}
		else if (collide(ball, guest))
		{
			bounce = true;
			ball.x = (guest.x + Paddle::width) - Ball::squishiness;
			ball.xv = -ball.xv;
			ball.yv = get_ball_yv(ball, guest);
		}

		// check for collision with area boundaries
		if (ball.x > area.right * 5.0f)
		{
            reset_serve(false);
			++networkdata.guest_score;
		}
		else if (ball.x < area.left * 5.0f)
		{
            reset_serve(true);
			++networkdata.host_score;
		}
		if ((ball.y + Ball::height) - Ball::squishiness > area.top)
		{
			bounce = true;
			ball.y = (area.top - Ball::height) + Ball::squishiness;
			ball.yv = -ball.yv;
		}
		else if (ball.y + Ball::squishiness < area.bottom)
		{
			bounce = true;
			ball.y = area.bottom - Ball::squishiness;
			ball.yv = -ball.yv;
		}

		networkdata.ball_x = ball.x;
		networkdata.ball_y = ball.y;
		networkdata.ball_xv = ball.xv;
		networkdata.ball_yv = ball.yv;
	}
	else
	{
		if (ball.xv != networkdata.ball_xv || ball.yv != networkdata.ball_yv)
			bounce = true;

		ball.x = networkdata.ball_x;
		ball.y = networkdata.ball_y;
		ball.xv = networkdata.ball_xv;
		ball.yv = networkdata.ball_yv;
	}

	if (bounce)
	{
		if (bounces.size() > 3)
			bounces.erase(bounces.end() - 1);

		bounces.emplace(bounces.begin(), ball.x, ball.y);
	}

	const auto distance = [](float x1, float y1, float x2, float y2)
	{
		return std::sqrtf(std::powf(x2 - x1, 2) + std::powf(y2 - y1, 2));
	};

	// position ball tail
	const float trail_distance = 0.06f;
    for (int i = 0; i < BallTailItem::tails; ++i)
    {
	    float dist = trail_distance + (trail_distance * i);
    	float x = ball.x, y = ball.y;

    	for (const auto &bounce_point : bounces)
    	{
    		const auto d = distance(x, y, bounce_point.x, bounce_point.y);
    		if (dist > d)
    		{
    			x = bounce_point.x;
    			y = bounce_point.y;
                dist = dist - d;
    		}
    		else
    		{
    			const float angle = std::atan2f(bounce_point.y - y, bounce_point.x - x);

    			tails[i].oldx = tails[i].x;
    			tails[i].oldy = tails[i].y;
    			tails[i].x = x + (std::cosf(angle) * dist);
    			tails[i].y = y + (std::sinf(angle) * dist);

			    renderables.emplace_back(0,
					Texture::ball,
					tails[i].x,
					tails[i].y,
					tails[i].oldx,
					tails[i].oldy,
					Ball::width,
					Ball::height,
					Ball::width,
					Ball::height,
					1.0f,
					win::Color<float>(1.0f, 0.0f, 0.0f, 1.0f),
					win::Color<float>(1.0f, 0.0f, 0.0f, 1.0f));

    			break;
    		}
    	}
    }

	// emit renderables
	renderables.emplace_back(
		0,
		Texture::background,
		area.left,
		area.bottom,
		area.left,
		area.bottom,
		area.right - area.left,
		area.top - area.bottom,
		area.right - area.left,
		area.top - area.bottom,
		1.0f,
		win::Color<float>(1.0f, 1.0f, 1.0f, 1.0f),
		win::Color<float>(0.0f, 0.0f, 0.0f, 0.0f));

	renderables.emplace_back(
		0,
		Texture::ball,
		ball.x,
		ball.y,
		oldx,
		oldy,
		Ball::width,
		Ball::height,
		Ball::width,
		Ball::height,
		1.0f,
		win::Color<float>(1, 0, 0, 1),
		win::Color<float>(1, 0, 0, 1));

	light_renderables.emplace_back(
		ball.x + (Ball::width / 2.0f),
		ball.y + (Ball::height / 2.0f),
		oldx + (Ball::width / 2.0f),
		oldy + (Ball::height / 2.0f),
		win::Color<float>(0.6f, 0.4f, 0.4f),
		100);
}

LerpedRenderable Game::process_player_paddle(const Input &input)
{
	if (runbot)
	{
		guest.y = (ball.y - (Paddle::height / 2.0f)) + (Ball::height / 2.0f);
		networkdata.guest_paddle_y = guest.y;
		networkdata.guest_paddle_color = (int)color;

		return LerpedRenderable(
			0,
			Texture::paddle,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			0,
			win::Color<float>(0, 0, 0, 0),
			win::Color<float>(0, 0, 0, 0)); // this isn't going to go anywhere anyway
	}
	else
	{
		auto &paddle = match.hosting() ? host : guest;
		auto &networky = match.hosting() ? networkdata.host_paddle_y : networkdata.guest_paddle_y;
		auto &networkcolor = match.hosting() ? networkdata.host_paddle_color : networkdata.guest_paddle_color;

		networkcolor = (int)color;

		const float oldy = paddle.y;
		paddle.y = input.y - (Paddle::height / 2.0f);
		networky = paddle.y;

		return LerpedRenderable(
			0,
			Texture::paddle,
			paddle.x,
			paddle.y,
			paddle.x,
			oldy,
			Paddle::width,
			Paddle::height,
			Paddle::width,
			Paddle::height,
			1.0f,
			get_color(color),
			get_color(color));
	}
}

LerpedRenderable Game::process_opponent_paddle()
{
	auto &paddle = match.hosting() ? guest : host;
	const auto y = match.hosting() ? networkdata.guest_paddle_y : networkdata.host_paddle_y;
	const auto c = (Color)(match.hosting() ? networkdata.guest_paddle_color : networkdata.host_paddle_color);

	const float oldy = paddle.y;
	paddle.y = y;

	return LerpedRenderable(
		0,
		Texture::paddle,
		paddle.x,
		paddle.y,
		paddle.x, oldy,
		Paddle::width,
		Paddle::height,
		Paddle::width,
		Paddle::height,
		1.0f,
		get_color(c),
		get_color(c));

}

void Game::reset_serve(bool towards_host)
{
	ball.x = -Ball::width / 2.0f;
	ball.y = -Ball::height / 2.0f;
	ball.xv = 0.35f * (towards_host ? 1.0f : -1.0f);
	ball.yv = 0.0f;

	bounces.clear();
	bounces.emplace_back(ball.x, ball.y);

	for (auto &tail : tails)
	{
		tail.x = ball.x;
		tail.y = ball.y;
		tail.oldx = tail.x;
		tail.oldy = tail.y;
	}
}

void Game::reset()
{
	ball.x = 0.0f;
	ball.y = 0.0f;
	ball.xv = 0.35f;
	ball.yv = 0.0f;

	bounces.clear();
	bounces.emplace_back(ball.x, ball.y);

	host.x = (area.right - Paddle::width) - 0.5f;
	host.y = 0.0f;

	guest.x = area.left + 0.5f;
	guest.y = 0.0f;
}

bool Game::collide(const Ball &ball, const Paddle &paddle)
{
	return
		ball.y + Ball::height > paddle.y && ball.y < paddle.y + Paddle::height &&
		(ball.x + Ball::width) - Ball::squishiness > paddle.x && ball.x + Ball::squishiness < paddle.x + (Paddle::width * 1.0f);
}

float Game::get_ball_yv(const Ball &ball, const Paddle &paddle)
{
	const float mult = ((((ball.y + (Ball::height / 2.0f)) - paddle.y) / Paddle::height) * 2.0f) - 1.0f;

	return mult / 5.0f;
}

