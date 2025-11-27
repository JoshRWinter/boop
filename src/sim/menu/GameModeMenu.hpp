#pragma once

#include <functional>

#include "../GameMode.hpp"
#include "Menu.hpp"

class GameModeMenu : public Menu
{
public:
	explicit GameModeMenu(std::function<void(GameMode m)> result);

private:
	std::function<void(GameMode m)> result;
};
