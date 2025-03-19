#pragma once

#include <random>

#include <win/Win.hpp>
#include <win/Utility.hpp>

#include "../Colors.hpp"
#include "../render/Renderable.hpp"
#include "../Input.hpp"
#include "NetworkMatch.hpp"
#include "SimulationHost.hpp"

struct Ball
{
	static constexpr float width = 0.3f;
	static constexpr float height = 0.3f;
	static constexpr float squishiness = width * 0.3f;

	float x, y;
	float xv, yv;
};

struct BallTailItem
{
	static constexpr int tails = 6;
	float x, oldx, y, oldy;
};

struct Paddle
{
    static constexpr float width = 0.3f;
    static constexpr float height = 1.5666f;

	float x, y;
};

class Game
{
	WIN_NO_COPY_MOVE(Game);

	struct NetworkData
	{
		int host_paddle_color = 0, guest_paddle_color = 0;
		float host_paddle_y = 0.0f, guest_paddle_y = 0.0f;
		float ball_x = 0.0f, ball_y = 0.0f;
		float ball_xv = 0.0f, ball_yv = 0.0f;
		int host_score = 0, guest_score = 0;
	};

public:
	Game(const win::Area<float> &area, bool runbot);

	void play(SimulationHost &host);

private:
	void tick(Renderables &renderables, const Input &input);
	void process_ball(std::vector<LerpedRenderable> &renderables, std::vector<LerpedLightRenderable> &light_renderables);
	LerpedRenderable process_player_paddle(const Input &input);
	LerpedRenderable process_opponent_paddle();
	void reset_serve(bool towards_host);
	void reset();
	bool collide(const Ball &ball, const Paddle &paddle);
	float get_ball_yv(const Ball &ball, const Paddle &paddle);

	std::mt19937 rand;
	bool showmenu;
	bool runbot;
	Color paddle_color;
	Color current_ball_color;
	Color target_ball_color;
	float ball_color_switch_pct;
	Ball ball;
	BallTailItem tails[BallTailItem::tails];
	Paddle host;
	Paddle guest;
	win::Area<float> area;
	NetworkMatch match;
	NetworkData networkdata;

	std::vector<win::Pair<float>> bounces;
};
