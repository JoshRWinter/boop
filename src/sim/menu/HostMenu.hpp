#pragma once

#include <functional>

#include "../NetworkMatch.hpp"
#include "Menu.hpp"

class HostMenu : public Menu
{
public:
	explicit HostMenu(NetworkMatch &match, std::function<void()> result);

private:
	std::function<void()> result;
	NetworkMatch &match;
	NetworkMatch::ErrorReason error = NetworkMatch::ErrorReason::none;
};
