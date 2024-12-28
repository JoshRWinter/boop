#pragma once

#include <vector>

#include <win/Win.hpp>
#include <win/Utility.hpp>

#include "../render/Renderable.hpp"
#include "../Input.hpp"

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

public:
	explicit Game(const win::Area<float> &area);

	void tick(std::vector<Renderable> &renderables, const Input &input);

private:
	void reset();
	bool collide(const Ball &ball, const Paddle &paddle);
	float get_ball_yv(const Ball &ball, const Paddle &paddle);

	Ball ball;
	Paddle host;
	Paddle guest;
	win::Area<float> area;
};
