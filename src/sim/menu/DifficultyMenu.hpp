#pragma once

#include <functional>

#include "../Difficulty.hpp"
#include "Menu.hpp"

class DifficultyMenu : public Menu
{
public:
	explicit DifficultyMenu(std::function<void(DifficultyLevel d)> result);

private:
	std::function<void(DifficultyLevel d)> result;
};
