#include "JoinMenu.hpp"
#include "JoiningMenu.hpp"

JoinMenu::JoinMenu(MenuService &menus, NetworkMatch &match, std::function<void()> result)
	: Menu("Type an IP address")
	, result(std::move(result))
	, menus(menus)
	, match(match)
{
	buttons.emplace_back(-Button::width / 2.0f,
						 -2.75f,
						 "Join",
						 (const char *)NULL,
						 [this]
						 {
							 this->match.reset();
							 this->menus.add(new JoiningMenu(this->match,
															 this->ip.c_str(),
															 [this]
															 {
																 this->result();
																 quit = true;
															 }));
						 });

	buttons.emplace_back(-Button::width / 2.0f,
						 -4.0f,
						 "Back",
						 (const char *)NULL,
						 [this]
						 {
							 quit = true;
						 });

	tick = [this](Renderables &renderables, const Input &input, const std::vector<char> &text_input)
	{
		if (!text_input.empty())
		{
			for (const auto c : text_input)
			{
				if (c == '\b')
				{
					if (!ip.empty())
						ip.erase(ip.end() - 1);
				}
				else
					ip += c;
			}
		}

		if (!ip.empty())
			renderables.text_renderables.emplace_back(0.0f, 0.0f, true, TextRenderable::Type::smol, win::Color(1.0f, 1.0f, 1.0f, 1.0f), ip.c_str());
	};
}
