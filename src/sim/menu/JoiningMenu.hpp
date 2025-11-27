#pragma once

#include <functional>

#include "../NetworkMatch.hpp"
#include "Menu.hpp"

class JoiningMenu : public Menu
{
public:
	explicit JoiningMenu(NetworkMatch &match, const char *ip, std::function<void()> result);

private:
	NetworkMatch &match;
	const char *ip;
	std::function<void()> result;
};
