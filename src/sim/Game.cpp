#include "Game.hpp"

Game::Game(const win::Area<float> &area, bool runbot)
	: menustate(runbot ? MenuState::none : MenuState::main)
	, runbot(runbot)
	, area(area)
{
	reset();

	if (runbot)
	{
		while (!match.join("::1")) ;
	}
}

void Game::play(Renderables &renderables, const Input &input)
{
	if (menustate == MenuState::main)
	{
		const auto result = main_menu.show(renderables, input, match);
		if (result == MainMenuResult::play)
			menustate = MenuState::none;
	}
	else
		tick(renderables, input);
}

void Game::tick(Renderables &renderables, const Input &input)
{
	if (match.hosting())
		match.host_get_data(networkdata.guest_paddle_y);
	else
		match.guest_get_data(networkdata.host_paddle_y, networkdata.ball_x, networkdata.ball_y, networkdata.host_score, networkdata.guest_score);

	renderables.lerped_renderables.push_back(process_ball());
	renderables.lerped_renderables.emplace_back(process_player_paddle(input));
	renderables.lerped_renderables.emplace_back(process_opponent_paddle());

	char scoretext[10];
	snprintf(scoretext, sizeof(scoretext), "%d", networkdata.guest_score);
	renderables.text_renderables.emplace_back(0, -4.0f, 3.0f, true, TextRenderable::Type::smol, win::Color<float>(0.6f, 0.6f, 0.6f, 1.0f), scoretext);
	snprintf(scoretext, sizeof(scoretext), "%d", networkdata.host_score);
	renderables.text_renderables.emplace_back(0, 4.0f, 3.0f, true, TextRenderable::Type::smol, win::Color<float>(0.6f, 0.6f, 0.6f, 1.0f), scoretext);

	if (match.hosting())
		match.host_send_data(networkdata.host_paddle_y, ball.x, ball.y, networkdata.host_score, networkdata.guest_score);
	else
		match.guest_send_data(networkdata.guest_paddle_y);
}

LerpedRenderable Game::process_ball()
{
	const float oldx = ball.x;
	const float oldy = ball.y;

	if (match.hosting())
	{
		ball.x += ball.xv;
		ball.y += ball.yv;

		// check for collision with paddles
		if (collide(ball, host))
		{
			ball.x = (host.x - Ball::width) + Ball::squishiness;
			ball.xv = -ball.xv;
			ball.yv = get_ball_yv(ball, host);
		}
		else if (collide(ball, guest))
		{
			ball.x = (guest.x + Paddle::width) - Ball::squishiness;
			ball.xv = -ball.xv;
			ball.yv = get_ball_yv(ball, guest);
		}

		// check for collision with area boundaries
		if (ball.x > area.right * 5.0f)
		{
			ball.x = -Ball::width / 2.0f;
			ball.y = -Ball::height / 2.0f;
			ball.xv = ball.xv = -0.35f;
			ball.yv = 0.0f;
			++networkdata.guest_score;
		}
		else if (ball.x < area.left * 5.0f)
		{
			ball.x = -Ball::width / 2.0f;
			ball.y = -Ball::height / 2.0f;
			ball.xv = 0.35f;
			ball.yv = 0.0f;
			++networkdata.host_score;
		}
		if ((ball.y + Ball::height) - Ball::squishiness > area.top)
		{
			ball.y = (area.top - Ball::height) + Ball::squishiness;
			ball.yv = -ball.yv;
		}
		else if (ball.y + Ball::squishiness < area.bottom)
		{
			ball.y = area.bottom - Ball::squishiness;
			ball.yv = -ball.yv;
		}
	}
	else
	{
		ball.x = networkdata.ball_x;
		ball.y = networkdata.ball_y;
	}

	if (match.hosting())
	{
		networkdata.ball_x = ball.x;
		networkdata.ball_y = ball.y;
	}

	return LerpedRenderable(0, Texture::ball, ball.x, ball.y, oldx, oldy, Ball::width, Ball::height, win::Color<float>(1, 0, 0, 1));
}

LerpedRenderable Game::process_player_paddle(const Input &input)
{
	if (runbot)
	{
		guest.y = (ball.y - (Paddle::height / 2.0f)) + (Ball::height / 2.0f);
		networkdata.guest_paddle_y = guest.y;
		return LerpedRenderable(0, Texture::paddle, 0, 0, 0, 0, 0, 0, win::Color<float>(0, 0, 0, 0)); // this isn't going to go anywhere anyway
	}
	else
	{
		auto &paddle = match.hosting() ? host : guest;
		auto &networky = match.hosting() ? networkdata.host_paddle_y : networkdata.guest_paddle_y;

		const float oldy = paddle.y;
		paddle.y = input.y - (Paddle::height / 2.0f);
		networky = paddle.y;
		return LerpedRenderable(0, Texture::paddle, paddle.x, paddle.y, paddle.x, oldy, Paddle::width, Paddle::height, win::Color<float>(1, 1, 1, 1));
	}
}

LerpedRenderable Game::process_opponent_paddle()
{
	auto &paddle = match.hosting() ? guest : host;
	const auto y = match.hosting() ? networkdata.guest_paddle_y : networkdata.host_paddle_y;

	const float oldy = paddle.y;
	paddle.y = y;
	return LerpedRenderable(0, Texture::paddle, paddle.x, paddle.y, paddle.x, oldy, Paddle::width, Paddle::height, win::Color<float>(1, 1, 1, 1));
}

void Game::reset()
{
	ball.x = 0.0f;
	ball.y = 0.0f;
	ball.xv = 0.35f;
	ball.yv = 0.0f;

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

