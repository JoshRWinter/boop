#include "Game.hpp"

Game::Game(const win::Area<float> &area)
	: area(area)
{
	reset();
}

void Game::tick(std::vector<Renderable> &renderables, const Input &input)
{
	const float oldx = ball.x, oldy = ball.y;

	ball.x += ball.xv;
	ball.y += ball.yv;

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

	/*
	if (ball.x < -2.0f)
	{
		ball.x = -2.0f;
		ball.y = 0.0f;
		ball.yv = 0.0f;
		ball.xv = 0.2f;
	}
	*/

	if ((ball.x + Ball::width) - Ball::squishiness > area.right)
	{
		ball.x = (area.right - Ball::width) + Ball::squishiness;
		ball.xv = -ball.xv;
	}
	else if (ball.x + Ball::squishiness < area.left)
	{
		ball.x = area.left - Ball::squishiness;
		ball.xv = -ball.xv;
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

	host.y = input.y - (Paddle::height / 2.0f);

	renderables.emplace_back(Texture::ball, ball.x, ball.y, oldx, oldy, Ball::width, Ball::height, win::Color<float>(1, 0, 1, 1));
	renderables.emplace_back(Texture::paddle, host.x, host.y, host.x, host.y, Paddle::width, Paddle::height, win::Color<float>(0, 1, 1, 1));
	renderables.emplace_back(Texture::paddle, guest.x, guest.y, guest.x, guest.y, Paddle::width, Paddle::height, win::Color<float>(0, 1, 1, 1));
}

void Game::reset()
{
	ball.x = 0.0f;
	ball.y = 0.0f;
	ball.xv = 0.35f;
	ball.yv = 0.15f;

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

