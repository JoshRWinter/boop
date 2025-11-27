#pragma once

#include <functional>

#include <win/Win.hpp>

#include "../../Input.hpp"
#include "../../render/Renderable.hpp"
#include "Button.hpp"

class Menu
{
	WIN_NO_COPY_MOVE(Menu);

public:
	virtual ~Menu() = default;

	std::vector<Button> buttons;
	const char *title;
	std::function<void(Renderables &, const Input &, const std::vector<char> &)> tick = [](Renderables &, const Input &, const std::vector<char> &) {};
	bool quit = false;

protected:
	explicit Menu(const char *title)
		: title(title)
	{
	}
};
