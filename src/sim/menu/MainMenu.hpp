#pragma once

#include <functional>

#include "../../Colors.hpp"
#include "../Difficulty.hpp"
#include "../GameMode.hpp"
#include "../NetworkMatch.hpp"
#include "Menu.hpp"
#include "MenuService.hpp"

class MainMenu : public Menu
{
	struct ColorSelect
	{
		constexpr static float size = 0.75f;

		ColorSelect(float x, float y, Color color)
			: x(x)
			, y(y)
			, color(color)
		{
		}

		float x, y;
		bool selected = false;
		Color color;
	};

public:
	MainMenu(MenuService &meenus, NetworkMatch &match, std::function<void(Color c, DifficultyLevel d, GameMode g)> result, std::function<void()> exit);

	const char *error = NULL;

private:
	MenuService &menus;
	NetworkMatch &match;
	std::function<void(Color c, DifficultyLevel d, GameMode g)> result;
	std::function<void()> exit;
	ColorSelect red, green, blue, yellow, cyan, magenta;
	ColorSelect *colors[6] {&red, &green, &blue, &yellow, &cyan, &magenta};
	Color current;
};
