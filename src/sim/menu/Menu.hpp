#pragma once

#include <win/Win.hpp>

#include "../SoundManager.hpp"
#include "../SimulationHost.hpp"
#include "../NetworkMatch.hpp"
#include "../../Colors.hpp"
#include "../Difficulty.hpp"

struct MainMenuResult
{
	MainMenuResult(Color color, DifficultyLevel difficulty)
		: color(color), difficulty(difficulty) {}

	Color color;
	DifficultyLevel difficulty;
};

class Menu
{
	Menu() = delete;
	WIN_NO_COPY_MOVE(Menu);

	static constexpr float button_width = 3.5f;
	static constexpr float button_height = 0.79166f;

	inline static auto textcolor = win::Color<float>(0.6f, 0.6f, 0.6f, 1.0f);

	inline static auto color = win::Color<float>(0.075f, 0.075f, 0.075f, 1.0f);
	inline static auto hover = win::Color<float>(0.4f, 0.05f, 0.05f, 1.0f);
	inline static auto clicked = win::Color<float>(0.3f, 0.02f, 0.02f, 1.0f);

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

	static MainMenuResult menu_main(SimulationHost &host, SoundManager &sounds, NetworkMatch &match, const win::Area<float> &area, const char *errmsg);
	static bool menu_choose_difficulty(SimulationHost &host, SoundManager &sounds, const win::Area<float> &area, Color color, DifficultyLevel &level);
	static bool menu_host(SimulationHost &host, SoundManager &sounds, NetworkMatch &match, NetworkMatch::ErrorReason &error, const win::Area<float> &area, Color color);
	static bool menu_join(SimulationHost &host, SoundManager &sounds, NetworkMatch &match, NetworkMatch::ErrorReason &error, const win::Area<float> &area, Color color);
	static bool menu_joining(SimulationHost &host, SoundManager &sounds, NetworkMatch &match, NetworkMatch::ErrorReason &error, const win::Area<float> &area, Color color, const char *ip);

private:
	static void map_renderables(Renderables &renderables, const Button &button, float x, float y);
	static void map_renderables(Renderables &renderables, const ColorSelect &color, bool selected);
	static void map_theme(Renderables &renderables, const win::Area<float> &area, const Input &input, Color color);
	static bool mouseover(const Button &button, float x, float y);
	static bool mouseover(const ColorSelect &color, float x, float y);
	static bool button_clicked(const Button &button, bool click, float x, float y);
};
