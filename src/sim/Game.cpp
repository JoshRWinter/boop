#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#include "Game.hpp"
#include "menu/Menu.hpp"

Game::Game(const win::Area<float> &area, bool runbot)
	: rand(time(NULL))
	, showmenu(true)
	, runbot(runbot)
	, background_renderable_id(next_renderable_id++)
	, ball(next_renderable_id++, next_renderable_id++)
	, host(next_renderable_id++)
	, guest(next_renderable_id++)
	, area(area)
	, match(area)
{
	paddle_color = runbot ? (Color)std::uniform_int_distribution<int>(0, (int)Color::last)(rand) : Color::red;
	reset();

	if (runbot)
	{
		while (!match.join("::1")) std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

void Game::play(SimulationHost &sim)
{
	std::vector<char> textinput(20);
	textinput.clear();

	while (!sim.quit())
	{
		if (showmenu && !runbot)
		{
			showmenu = false;
			const auto result = Menu::menu_main(sim, match, "");
			paddle_color = result.color;
			difficulty = result.difficulty;

			if (sim.quit())
				return;
		}

		auto &renderables = sim.get_renderables();
		const auto input = sim.get_input();
		sim.get_text_input(textinput);

		tick(renderables, input);

		sim.release_renderables(renderables);

		sim.sleep();
	}
}

void Game::tick(Renderables &renderables, const Input &input)
{
	++match_time;

	if (match.hosting())
		match.host_get_data(networkdata.guest_paddle_color, networkdata.guest_paddle_y);
	else
		match.guest_get_data(networkdata.host_paddle_color, networkdata.host_paddle_y, networkdata.paddle_height, networkdata.ball_x, networkdata.ball_y, networkdata.ball_xv, networkdata.ball_yv, networkdata.host_score, networkdata.guest_score);

	const auto reason = match.errored();
	if (reason != NetworkMatch::ErrorReason::none)
	{
		match.reset();
		showmenu = true;
		return;
	}

	renderables.renderables.emplace_back(
        background_renderable_id,
		Texture::background,
		area.left,
		area.bottom,
		area.right - area.left,
		area.top - area.bottom,
		0.0f,
		0.0f,
		win::Color<float>(1.0f, 1.0f, 1.0f, 1.0f),
		win::Color<float>(0.0f, 0.0f, 0.0f, 0.0f));

	process_ball(renderables.renderables, renderables.light_renderables);
	renderables.renderables.emplace_back(process_player_paddle(input));
	renderables.renderables.emplace_back(process_opponent_paddle());

	renderables.player_controlled_id = match.hosting() ? host.renderable_id : guest.renderable_id;

	char scoretext[10];
	snprintf(scoretext, sizeof(scoretext), "%d", networkdata.guest_score);
	renderables.text_renderables.emplace_back(-4.0f, 3.0f, true, TextRenderable::Type::smol, win::Color<float>(0.6f, 0.6f, 0.6f, 1.0f), scoretext);
	snprintf(scoretext, sizeof(scoretext), "%d", networkdata.host_score);
	renderables.text_renderables.emplace_back(4.0f, 3.0f, true, TextRenderable::Type::smol, win::Color<float>(0.6f, 0.6f, 0.6f, 1.0f), scoretext);

	if (match.hosting())
		match.host_send_data(networkdata.host_paddle_color, networkdata.host_paddle_y, networkdata.paddle_height, networkdata.ball_x, networkdata.ball_y, networkdata.ball_xv, networkdata.ball_yv, networkdata.host_score, networkdata.guest_score);
	else
		match.guest_send_data(networkdata.guest_paddle_color, networkdata.guest_paddle_y);

	const auto reason2 = match.errored();
	if (reason2 != NetworkMatch::ErrorReason::none)
	{
		match.reset();
		showmenu = true;
	}
}

void Game::process_ball(std::vector<Renderable> &renderables, std::vector<LightRenderable> &light_renderables)
{
	win::Color<float> ball_color;

	{
		const auto lerp = [](float a, float b, float t) { return ((b - a) * std::clamp(t, 0.0f, 1.0f)) + a; };

		const auto current_color = get_color(current_ball_color);
		const auto next_color = get_color(target_ball_color);

		ball_color.red = lerp(current_color.red, next_color.red, ball_color_switch_pct);
		ball_color.green = lerp(current_color.green, next_color.green, ball_color_switch_pct);
		ball_color.blue = lerp(current_color.blue, next_color.blue, ball_color_switch_pct);

		ball_color_switch_pct += 0.001f;

		if (ball_color_switch_pct > 1.5)
		{
			ball_color_switch_pct = 0.0f;
			current_ball_color = target_ball_color;
			target_ball_color = (Color)std::uniform_int_distribution<int>(0, (int)Color::last)(rand);
		}
	}

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
			get_ball_bounce(ball, host, get_speed(), ball.xv, ball.yv);
		}
		else if (collide(ball, guest))
		{
			bounce = true;
			ball.x = (guest.x + Paddle::width) - Ball::squishiness;
			ball.xv = -ball.xv;
			get_ball_bounce(ball, guest, get_speed(), ball.xv, ball.yv);
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
	const float trail_distance = 0.09f;
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

    			tails[i].x = x + (std::cosf(angle) * dist);
    			tails[i].y = y + (std::sinf(angle) * dist);

			    renderables.emplace_back(
			    	tails[i].renderable_id,
					Texture::ball,
					tails[i].x,
					tails[i].y,
					Ball::width,
					Ball::height,
					0.0f,
					1.0f,
					ball_color,
					ball_color);

    			break;
    		}
    	}
    }

	// emit renderables
	renderables.emplace_back(
        ball.renderable_id,
		Texture::ball,
		ball.x,
		ball.y,
		Ball::width,
		Ball::height,
		0.0f,
		1.0f,
		ball_color,
        ball_color);

	light_renderables.emplace_back(
        ball.light_renderable_id,
		ball.x + (Ball::width / 2.0f),
		ball.y + (Ball::height / 2.0f),
		ball_color,
		220);
}

Renderable Game::process_player_paddle(const Input &input)
{
	if (runbot)
	{
		guest.y = (ball.y - (guest.h / 2.0f)) + (Ball::height / 2.0f);
		guest.h = networkdata.paddle_height;
		networkdata.guest_paddle_y = guest.y + (guest.h / 2.0f);
		networkdata.guest_paddle_color = (int)paddle_color;

		return Renderable(
			0,
			Texture::paddle,
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

		paddle.h = match.hosting() ? get_paddle_height() : networkdata.paddle_height;

		if (match.hosting())
			networkdata.paddle_height = paddle.h;

		networkcolor = (int)paddle_color;

		paddle.y = input.y - (paddle.h / 2.0f);
		networky = input.y;

		return Renderable(
            paddle.renderable_id,
			Texture::paddle,
			paddle.x,
			paddle.y,
			Paddle::width,
			paddle.h,
            paddle.x > 0.0f ? 0.0f : M_PI,
			1.0f,
			get_color(paddle_color),
			get_color(paddle_color));
	}
}

Renderable Game::process_opponent_paddle()
{
	auto &paddle = match.hosting() ? guest : host;
	paddle.h = match.hosting() ? get_paddle_height() : networkdata.paddle_height;
	const auto y = match.hosting() ? networkdata.guest_paddle_y : networkdata.host_paddle_y;
	const auto c = (Color)(match.hosting() ? networkdata.guest_paddle_color : networkdata.host_paddle_color);

	paddle.y = y - (paddle.h / 2.0f);

	return Renderable(
        paddle.renderable_id,
		Texture::paddle,
		paddle.x,
		paddle.y,
		Paddle::width,
		paddle.h,
        paddle.x > 0.0f ? 0.0f : M_PI,
		1.0f,
		get_color(c),
		get_color(c));
}

void Game::reset_serve(bool towards_host)
{
	match_time = 0;

	ball.renderable_id = next_renderable_id++;

	ball.yv = 0.0f;
	ball.xv = get_speed() * (towards_host ? 1.0f : -1.0f);

	ball.x = -Ball::width / 2.0f;
	ball.y = -Ball::height / 2.0f;

	bounces.clear();
	bounces.emplace_back(ball.x, ball.y);

	for (auto &tail : tails)
	{
		tail.renderable_id = next_renderable_id++;
		tail.x = ball.x;
		tail.y = ball.y;
	}
}

void Game::reset()
{
	reset_serve(true);

	host.x = (area.right - (Paddle::width / 2.0f)) - 0.3f;
	host.y = 0.0f;

	guest.x = (area.left + 0.3f) - (Paddle::width / 2.0f);
	guest.y = 0.0f;

	current_ball_color = (Color)std::uniform_int_distribution<int>(0, (int)Color::last)(rand);
	target_ball_color = (Color)std::uniform_int_distribution<int>(0, (int)Color::last)(rand);
	ball_color_switch_pct = 0.0f;
}

bool Game::collide(const Ball &ball, const Paddle &paddle)
{
	return
		ball.y + Ball::height > paddle.y && ball.y < paddle.y + paddle.h &&
		(ball.x + Ball::width) - Ball::squishiness > paddle.x && ball.x + Ball::squishiness < paddle.x + (Paddle::width * 1.0f);
}

float Game::get_paddle_height()
{
	const float min_height = Difficulty::get_min_paddle_height(difficulty);
	const float max_height = 2.0f;
	const float progression = std::min(match_time / 2000.0f, 1.0f);

	return ((min_height - max_height) * progression) + max_height;
}

float Game::get_speed()
{
	const float min_speed = 0.275f;
	const float max_speed = Difficulty::get_max_ball_speed(difficulty);
	const float progression = std::min(match_time / 1200.0f, 1.0f);

	return ((max_speed - min_speed) * progression) + min_speed;
}

void Game::get_ball_bounce(const Ball &ball, const Paddle &paddle, float speed, float &xv, float &yv)
{
	const float angle_high = paddle.x > 0.0f ? 2.3562f : 0.7854f;
	const float angle_low = paddle.x > 0.0f ? 3.927f : -0.7854;

	const float paddle_center_y = paddle.y + (paddle.h / 2.0f);
	const float ball_center_y = (ball.y + (Ball::height / 2.0f)) - paddle_center_y;

	const float paddle_top = paddle.h / 2.0f;

	// between -1 and 1 based on the spot the ball hits the paddle
	// (may be slightly more or less than -1 or 1)
	const float ypos = ball_center_y / paddle_top;

	// the same number expressed on a scale from 0 to 1 (may be slightly more or less)
	float scale = (ypos + 1) / 2.0f;

	const float angle = ((angle_high - angle_low) * scale) + angle_low;

	xv = std::cosf(angle) * speed;
	yv = std::sinf(angle) * speed;
}

