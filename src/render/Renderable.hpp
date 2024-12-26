#pragma once

#include <win/Utility.hpp>

#include "../Texture.hpp"

struct Renderable
{
	Renderable(Texture texture, float x, float y, float w, float h, const win::Color<float> &color)
		: texture(texture), x(x), y(y), w(w), h(h), color(color) {}

	Texture texture;
	float x, y, w, h;
	win::Color<float> color;
};
