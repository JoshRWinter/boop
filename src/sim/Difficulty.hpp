#pragma once

enum class DifficultyLevel
{
	easy,
	med,
	hard
};

class Difficulty
{
public:
	Difficulty() = delete;

	static float get_min_paddle_height(DifficultyLevel level);
	static float get_max_ball_speed(DifficultyLevel level);
};
