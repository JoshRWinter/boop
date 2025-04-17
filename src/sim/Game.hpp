#pragma once

#include <random>

#include <win/Win.hpp>
#include <win/Utility.hpp>

#include "../Colors.hpp"
#include "../render/Renderable.hpp"
#include "../Input.hpp"
#include "NetworkMatch.hpp"
#include "SimulationHost.hpp"
#include "Difficulty.hpp"

struct Ball
{
	static constexpr float width = 0.24f;
	static constexpr float height = 0.24f;
	static constexpr float squishiness = width * 0.3f;

	Ball(unsigned renderable_id, unsigned light_renderable_id) : renderable_id(renderable_id), light_renderable_id(light_renderable_id) {}

	unsigned renderable_id;
	unsigned light_renderable_id;
	float x = 0.0f, y = 0.0f;
	float xv = 0.0f, yv = 0.0f;
};

struct BallTailItem
{
	static constexpr int tails = 4;
	unsigned renderable_id = -1;
	float x = 0.0f, y = 0.0f;
};

struct Paddle
{
    static constexpr float width = 0.6f;

    explicit Paddle(unsigned renderable_id) : renderable_id(renderable_id) {}

	unsigned renderable_id;
	float x = 0.0f, y = 0.0f, h = 0.0f;
};

class Game
{
	WIN_NO_COPY_MOVE(Game);

	struct NetworkData
	{
		int host_paddle_color = 0, guest_paddle_color = 0;
		float host_paddle_y = 0.0f, guest_paddle_y = 0.0f;
		float paddle_height = 0.0f;
		float ball_x = 0.0f, ball_y = 0.0f;
		float ball_xv = 0.0f, ball_yv = 0.0f;
		int host_score = 0, guest_score = 0;
	};

public:
	Game(const win::Area<float> &area, bool runbot);

	void play(SimulationHost &host);

private:
	void tick(Renderables &renderables, const Input &input);
	void process_ball(std::vector<Renderable> &renderables, std::vector<LightRenderable> &light_renderables);
	Renderable process_player_paddle(const Input &input);
	Renderable process_opponent_paddle();
	void reset_serve(bool towards_host);
	void reset();
	bool collide(const Ball &ball, const Paddle &paddle);
	float get_paddle_height();
	float get_speed();
	void get_ball_bounce(const Ball &ball, const Paddle &paddle, float speed, float &xv, float &yv);

	unsigned next_renderable_id = 0;
	std::mt19937 rand;
	bool showmenu;
	bool runbot;
	int match_time;
	unsigned background_renderable_id;
	DifficultyLevel difficulty;
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
