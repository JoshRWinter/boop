#pragma once

#include <win/Win.hpp>

#include "../SimulationHost.hpp"
#include "../NetworkMatch.hpp"
#include "../../Colors.hpp"

class Menu
{
	Menu() = delete;
	WIN_NO_COPY_MOVE(Menu);

	static constexpr float button_width = 3.0f;
	static constexpr float button_height = 0.8f;

	inline static auto textcolor = win::Color<float>(0.8f, 0.6f, 0.6f, 1.0f);

	inline static auto color = win::Color<float>(0.8f, 0.4f, 0.4f, 1.0f);
	inline static auto hover = win::Color<float>(0.6f, 0.2f, 0.2f, 1.0f);
	inline static auto clicked = win::Color<float>(0.4f, 0.1f, 0.1f, 1.0f);

public:
	struct Button
	{
		Button(float x, float y, float w, float h, const char *text)
			: x(x), y(y), w(w), h(h), text(text), click(false)
		{ }

		float x, y, w, h;
		const char *text;
		bool click;
	};

	struct ColorSelect
	{
		ColorSelect(float x, float y, float w, float h, const win::Color<float> &color)
			: x(x)
			, y(y)
			, w(w)
			, h(h)
			, color(color)
		{}

		float x, y, w, h;
		win::Color<float> color;
	};

	static Color menu_main(SimulationHost &host, NetworkMatch &match, const char *errmsg);
	static bool menu_host(SimulationHost &host, NetworkMatch &match, NetworkMatch::ErrorReason &error);
	static bool menu_join(SimulationHost &host, NetworkMatch &match, NetworkMatch::ErrorReason &error);
	static bool menu_joining(SimulationHost &host, NetworkMatch &match, NetworkMatch::ErrorReason &error, const char *ip);

private:
	static void map_renderables(Renderables &renderables, const Button &button, float x, float y);
	static void map_renderables(Renderables &renderables, const ColorSelect &color, bool selected);
	static bool mouseover(const Button &button, float x, float y);
	static bool mouseover(const ColorSelect &color, float x, float y);
	static bool button_clicked(const Button &button, bool click, float x, float y);
};
