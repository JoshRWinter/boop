#pragma once

#include <vector>

#include <win/Win.hpp>
#include <win/Utility.hpp>

#include "../render/Renderable.hpp"
#include "../Input.hpp"
#include "../NetworkMatch.hpp"

struct Ball
{
	static constexpr float width = 0.3f;
	static constexpr float height = 0.3f;
	static constexpr float squishiness = width * 0.3f;

	float x, y;
	float xv, yv;
};

struct Paddle
{
    static constexpr float width = 0.3f;
    static constexpr float height = 1.5f;

	float x, y;
};

class Game
{
	WIN_NO_COPY_MOVE(Game);

	struct NetworkData
	{
		float host_paddle_y = 0.0f, guest_paddle_y = 0.0f;
		float ball_x = 0.0f, ball_y = 0.0f;
		int host_score = 0, guest_score = 0;
	};

public:
	explicit Game(const win::Area<float> &area, bool is_host, bool runbot);

	void tick(std::vector<Renderable> &renderables, const Input &input);

private:
	Renderable process_ball();
	Renderable process_player_paddle(const Input &input);
	Renderable process_opponent_paddle();
	void reset();
	bool collide(const Ball &ball, const Paddle &paddle);
	float get_ball_yv(const Ball &ball, const Paddle &paddle);

	bool ishost;
	bool runbot;
	Ball ball;
	Paddle host;
	Paddle guest;
	win::Area<float> area;
	NetworkMatch match;
	NetworkData networkdata;
};
