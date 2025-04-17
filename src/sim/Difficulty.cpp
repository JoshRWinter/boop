#include <win/Win.hpp>

#include "Difficulty.hpp"

float Difficulty::get_max_ball_speed(DifficultyLevel level)
{
	switch (level)
	{
		case DifficultyLevel::easy:
			return 0.3f;
		case DifficultyLevel::med:
			return 0.38f;
		case DifficultyLevel::hard:
			return 0.45f;
		default: win::bug("wut");
	}
}

float Difficulty::get_min_paddle_height(DifficultyLevel level)
{
	switch (level)
	{
		case DifficultyLevel::easy:
			return 1.8f;
		case DifficultyLevel::med:
			return 1.5f;
		case DifficultyLevel::hard:
			return 1.3f;
		default: win::bug("wut");
	}
}

