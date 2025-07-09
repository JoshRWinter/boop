#pragma once

#include <vector>
#include <chrono>

#include <win/Utility.hpp>

#include "../Texture.hpp"

struct LightRenderable
{
	LightRenderable(unsigned id, float x, float y, const win::Color<float> &color, float power)
		: id(id)
		, x(x)
		, y(y)
		, color(color)
		, power(power) {}

	unsigned id;
	float x, y;
	win::Color<float> color;
	float power;
};

struct Renderable
{
	Renderable(unsigned id, Texture texture, float x, float y, float w, float h, float rot, float emissiveness, const win::Color<float> &color, const win::Color<float> &history_color)
		: id(id), texture(texture), x(x), y(y), w(w), h(h), rot(rot), emissiveness(emissiveness), color(color), history_color(history_color) {}

	unsigned id;
	Texture texture;
	float x, y, w, h;
	float rot;
	float emissiveness;
	win::Color<float> color;
	win::Color<float> history_color;
};

struct TextRenderable
{
	enum class Type { smol, yuge };

	TextRenderable(float x, float y, bool centered, Type texttype, const win::Color<float> &color, const char *text)
		: x(x)
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

	float x, y;
	bool centered;
	Type texttype;
	win::Color<float> color;
	char text[50];
};

struct MenuRenderable
{
	MenuRenderable(MenuTexture texture, float x, float y, float w, float h, const win::Color<float> &color)
		: texture(texture)
		, x(x)
		, y(y)
		, w(w)
		, h(h)
		, color(color)
	{}

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
		light_renderables.clear();
		menu_renderables.clear();
		text_renderables.clear();
	}

	std::vector<Renderable> renderables;
	std::vector<LightRenderable> light_renderables;
	std::vector<MenuRenderable> menu_renderables;
	std::vector<TextRenderable> text_renderables;

	unsigned player_controlled_id;
};
