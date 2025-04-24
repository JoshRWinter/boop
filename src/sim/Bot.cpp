#include <cmath>
#include <ctime>

#include "Bot.hpp"

Bot::Bot(const win::Area<float> &game_area, float ball_size, float ball_squishiness, float paddle_plane)
	: mersenne(time(NULL))
	  , game_area(game_area)
	  , ball_size(ball_size)
	  , ball_squishiness(ball_squishiness)
	  , paddle_plane(paddle_plane)
{
}

float Bot::run(float ball_x, float ball_y, float ball_xv, float ball_yv, float paddle_height, DifficultyLevel difficulty, int gametime)
{
	if (ball_xv < 0.0f)
	{
		if (!incoming)
		{
			// predict where the ball will end up

			float x = ball_x, y = ball_y;
			float reverse = 1.0f;

			while (x > paddle_plane)
			{
				x += ball_xv;
				y += ball_yv * reverse;

				if ((y + ball_size) - ball_squishiness > game_area.top)
				{
					y = (game_area.top - ball_size) + ball_squishiness;
					reverse = -reverse;
				}
				else if (y + ball_squishiness < game_area.bottom)
				{
					y = game_area.bottom - ball_squishiness;
					reverse = -reverse;
				}
			}

			ball_destination_y = y + (ball_size / 2.0f);
			paddle_y_offset = std::uniform_real_distribution<float>(-0.875f, 0.875f)(mersenne);
			paddle_speed = get_paddle_speed(difficulty);
		}

		incoming = true;
	}
	else
	{
		incoming = false;
	}

	const float halfpaddle = paddle_height / 2.0f;
	const float paddle_center_target = ball_destination_y - halfpaddle;
	const float paddle_target = paddle_center_target + (halfpaddle * paddle_y_offset);

	if (paddle_y > paddle_target)
	{
		paddle_y -= paddle_speed;

		if (paddle_y < paddle_target)
			paddle_y = paddle_target;
	}
	else if (paddle_y < paddle_target)
	{
		paddle_y += paddle_speed;

		if (paddle_y > paddle_target)
			paddle_y = paddle_target;
	}

	return paddle_y;
}

float Bot::get_paddle_speed(DifficultyLevel difficulty)
{
	switch (difficulty)
	{
		case DifficultyLevel::easy:
			return rando(0, 2) ? rando(0.08f, 0.1f) : rando(0.08f, 0.085f);
		case DifficultyLevel::med:
			return rando(0, 2) ? rando(0.1f, 0.14f) : rando(0.1f, 0.105f);
		case DifficultyLevel::hard:
			return rando(0, 1) ? rando(0.14f, 0.17f) : 0.14f;
		default: win::bug("nope");
	}
}

int Bot::rando(int min, int max)
{
	return std::uniform_int_distribution<int>(min, max)(mersenne);
}

float Bot::rando(float min, float max)
{
	return std::uniform_real_distribution<float>(min, max)(mersenne);
}
