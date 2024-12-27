#include "Game.hpp"

Game::Game(const win::Area<float> &area)
	: area(area)
{
	reset();
}

void Game::tick(std::vector<Renderable> &renderables)
{
	const float oldx = ball.x, oldy = ball.y;

	ball.x += ball.xv;
	ball.y += ball.yv;

	if (ball.x + Ball::width > area.right)
	{
		ball.x = area.right - Ball::width;
		ball.xv = -ball.xv;
	}
	else if (ball.x < area.left)
	{
		ball.x = area.left;
		ball.xv = -ball.xv;
	}
	if (ball.y + Ball::height > area.top)
	{
		ball.y = area.top - Ball::height;
		ball.yv = -ball.yv;
	}
	else if (ball.y < area.bottom)
	{
		ball.y = area.bottom;
		ball.yv = -ball.yv;
	}

	renderables.emplace_back(Texture::ball, ball.x, ball.y, oldx, oldy, Ball::width, Ball::height, win::Color<float>(1, 0, 1, 1));
}

void Game::reset()
{
	ball.x = 0.0f;
	ball.y = 3.0f;
	ball.xv = 0.1f;
	ball.yv = 0.11f;
}

