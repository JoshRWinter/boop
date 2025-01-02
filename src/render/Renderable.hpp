#pragma once

#include <vector>
#include <chrono>

#include <win/Utility.hpp>

#include "../Texture.hpp"

struct Renderable
{
	Renderable(int layer, Texture texture, float x, float y, float w, float h, const win::Color<float> &color)
		: layer(layer), texture(texture), x(x), y(y), w(w), h(h), color(color) {}

	int layer;
	Texture texture;
	float x, y, w, h;
	win::Color<float> color;
};

struct LerpedRenderable : Renderable
{
	LerpedRenderable(int layer, Texture texture, float x, float y, float oldx, float oldy, float w, float h, const win::Color<float> &color)
		: Renderable(layer, texture, x, y, w, h, color)
		, oldx(oldx)
		, oldy(oldy)
	{}

	float oldx, oldy;
};

struct MenuRenderable
{
	MenuRenderable(Texture texture, float x, float y, float w, float h, const win::Color<float> &color, const char *text, const win::Color<float> &textcolor);

	const char *text;
	win::Color<float> textcolor;
};

struct Renderables
{
	void clear()
	{
		renderables.clear();
		lerped_renderables.clear();
		menu_renderables.clear();
	}

	std::vector<Renderable> renderables;
	std::vector<LerpedRenderable> lerped_renderables;
	std::vector<MenuRenderable> menu_renderables;

	std::chrono::high_resolution_clock::time_point time;
};
