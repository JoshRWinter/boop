#pragma once

#include <optional>
#include <random>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>
#include <win/Win.hpp>

#include "../Colors.hpp"
#include "../Input.hpp"
#include "../render/Renderable.hpp"
#include "Bot.hpp"
#include "Difficulty.hpp"
#include "GameMode.hpp"
#include "menu/MainMenu.hpp"
#include "menu/MenuService.hpp"
#include "NetworkMatch.hpp"
#include "SoundManager.hpp"
#include "WinState.hpp"

struct Ball
{
	static constexpr float width = 0.20833f;
	static constexpr float height = 0.20833f;
	static constexpr float squishiness = width * 0.3f;

	Ball(unsigned renderable_id, unsigned light_renderable_id)
		: renderable_id(renderable_id)
		, light_renderable_id(light_renderable_id)
	{
	}

	unsigned renderable_id;
	unsigned light_renderable_id;
	float x = 0.0f, y = 0.0f;
	float xv = 0.0f, yv = 0.0f;
};

struct BallTailItem
{
	static constexpr int tails = 12;
	unsigned renderable_id = -1;
	float x = 0.0f, y = 0.0f;
};

struct Paddle
{
	static constexpr float width = 0.6f;

	explicit Paddle(unsigned renderable_id)
		: renderable_id(renderable_id)
	{
	}

	unsigned renderable_id;
	float x = 0.0f, y = 0.0f, h = 0.0f;
};

class Game
{
	WIN_NO_COPY_MOVE(Game);

	struct NetworkData
	{
		WinState winstate = WinState::playing;
		int host_paddle_color = 0, guest_paddle_color = 0;
		float host_paddle_y = 0.0f, guest_paddle_y = 0.0f;
		float paddle_height = 0.0f;
		float ball_x = 0.0f, ball_y = 0.0f;
		float ball_xv = 0.0f, ball_yv = 0.0f;
		int host_score = 0, guest_score = 0;
	};

public:
	Game(win::AssetRoll *roll, const win::Area<float> &area, bool runbot, DifficultyLevel bot_difficulty);

	bool play(Renderables &renderables, const Input &input, const std::vector<char> &text_input);

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

	unsigned next_renderable_id = 1;
	std::mt19937 rand;
	bool showmenu;
	bool runbot;
	int match_time = 0;
	unsigned background_renderable_id;
	Bot bot;
	DifficultyLevel difficulty = DifficultyLevel::easy;
	Color paddle_color;
	Color current_ball_color;
	Color target_ball_color;
	float ball_color_switch_pct = 0.0f;
	WinState winstate = WinState::playing;
	GameMode mode = GameMode::ten;
	int wintimer = 0;
	Ball ball;
	BallTailItem tails[BallTailItem::tails];
	Paddle host;
	Paddle guest;
	int host_score = 0, guest_score = 0;
	bool host_scored = false, guest_scored = false;
	int host_score_inarow = 0;
	int guest_score_inarow = 0;
	win::Area<float> area;
	NetworkMatch match;
	NetworkData networkdata;
	std::optional<SoundManager> sounds;
	std::optional<MenuService> menu;
	MainMenu *mainmenu;
	bool exit = false;

	std::vector<win::Pair<float>> bounces;
};
