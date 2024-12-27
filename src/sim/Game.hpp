#pragma once

#include <vector>

#include <win/Win.hpp>
#include <win/Utility.hpp>

#include "../render/Renderable.hpp"

struct Ball
{
	static constexpr float width = 0.3f;
	static constexpr float height = 0.3f;

	float x, y;
	float xv, yv;
};

class Game
{
	WIN_NO_COPY_MOVE(Game);

public:
	explicit Game(const win::Area<float> &area);

	void tick(std::vector<Renderable> &renderables);

private:
	void reset();

	Ball ball;
	win::Area<float> area;
};
