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
	LerpedRenderable(int layer, Texture texture, float x, float y, float old_x, float old_y, float w, float h, float old_w, float old_h, const win::Color<float> &color)
		: Renderable(layer, texture, x, y, w, h, color)
		, old_x(old_x)
		, old_y(old_y)
		, old_width(old_w)
		, old_height(old_h)
	{}

	float old_x, old_y, old_width, old_height;
};

struct TextRenderable
{
	enum class Type { smol, yuge };

	TextRenderable(int layer, float x, float y, bool centered, Type texttype, const win::Color<float> &color, const char *text)
		: layer(layer)
		, x(x)
		, y(y)
		, centered(centered)
		, texttype(texttype)
		, color(color)
	{
		int i;
		for (i = 0; i < sizeof(this->text) - 1; ++i)
		{
			if (text[i] == 0) break;
			this->text[i] = text[i];
		}
		this->text[i] = 0;
	}

	int layer;
	float x, y;
	bool centered;
	Type texttype;
	win::Color<float> color;
	char text[50];
};

struct MenuRenderable
{
	MenuRenderable(int layer, MenuTexture texture, float x, float y, float w, float h, const win::Color<float> &color)
		: layer(layer)
		, texture(texture)
		, x(x)
		, y(y)
		, w(w)
		, h(h)
		, color(color)
	{}

	int layer;
	MenuTexture texture;
	float x, y;
	float w, h;
	win::Color<float> color;
};

struct Renderables
{
	void clear()
	{
		renderables.clear();
		lerped_renderables.clear();
		menu_renderables.clear();
		text_renderables.clear();
	}

	std::vector<Renderable> renderables;
	std::vector<LerpedRenderable> lerped_renderables;
	std::vector<MenuRenderable> menu_renderables;
	std::vector<TextRenderable> text_renderables;

	std::chrono::high_resolution_clock::time_point time;
};
