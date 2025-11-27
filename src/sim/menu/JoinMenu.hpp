#pragma once

#include <functional>
#include <string>

#include "../NetworkMatch.hpp"
#include "Menu.hpp"
#include "MenuService.hpp"

class JoinMenu : public Menu
{
public:
	explicit JoinMenu(MenuService &menus, NetworkMatch &match, std::function<void()> result);

private:
	std::function<void()> result;
	MenuService &menus;
	NetworkMatch &match;
	std::string ip;
};
