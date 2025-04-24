#pragma once

#include <random>

#include <win/Win.hpp>
#include <win/Utility.hpp>

#include "Difficulty.hpp"

class Bot
{
	WIN_NO_COPY_MOVE(Bot);
public:
	explicit Bot(const win::Area<float> &game_area, float ball_size, float ball_squishiness, float paddle_plane);

	float run(float ball_x, float ball_y, float ball_xv, float ball_yv, float paddle_height, DifficultyLevel difficulty, int gametime);

private:
	float get_paddle_speed(DifficultyLevel difficulty);
	int rando(int min, int max);
	float rando(float min, float max);

	std::mt19937 mersenne;

	const win::Area<float> game_area;
	const float ball_size;
	const float ball_squishiness;
	const float paddle_plane;

	bool incoming = false;
	float ball_destination_y = 0.0f;
	float paddle_y = 0.0f;
	float paddle_y_offset = 0.0f;
	float paddle_speed = 0.0f;
};
