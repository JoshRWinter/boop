#pragma once

#include <vector>
#include <chrono>

#include <win/Utility.hpp>

#include "../Texture.hpp"

struct Renderable
{
	Renderable(Texture texture, float x, float y, float oldx, float oldy, float w, float h, const win::Color<float> &color)
		: texture(texture), x(x), y(y), oldx(oldx), oldy(oldy), w(w), h(h), color(color) {}

	Texture texture;
	float x, y, oldx, oldy, w, h;
	win::Color<float> color;
};

struct RenderableCollection
{
	std::vector<Renderable> renderables;
	std::chrono::high_resolution_clock::time_point time;
};
