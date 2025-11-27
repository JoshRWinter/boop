#include <cstring>

#include "MenuService.hpp"

MenuService::MenuService(SoundManager &sounds)
	: sounds(sounds)
{
}

void MenuService::run(Renderables &renderables, const Input &input, const std::vector<char> &text_input)
{
	// prune all the kaput menus
	for (auto it = menustack.begin(); it != menustack.end();)
	{
		if (it->get()->quit)
			it = menustack.erase(it);
		else
			++it;
	}

	if (menustack.empty())
		win::bug("Menu stack is empty");

	Menu &menu = *menustack.back().get();

	// process buttons
	for (auto &button : menu.buttons)
	{
		const bool mouseover = input.x > button.x && input.x < button.x + Button::width && input.y > button.y && input.y < button.y + Button::height;

		if (mouseover && button.click && !input.click)
		{
			sounds.play_menu_click();

			button.callback();
			button.click = false;

			// often, the menu stack changes after a button callback (new menu being setup). Just go ahead and restart the menu run after a button callback.
			run(renderables, input, text_input); // should get tail call optimized?
			return;
		}

		button.click = mouseover && input.click;
	}

	// call menu tick
	menu.tick(renderables, input, text_input);

	// draw buttons
	for (const auto &button : menu.buttons)
	{
		const bool mouseover = input.x > button.x && input.x < button.x + Button::width && input.y > button.y && input.y < button.y + Button::height;
		if (mouseover && button.desc != NULL && strlen(button.desc) > 0)
			renderables.text_renderables.emplace_back(5.0f, -1.5f, true, TextRenderable::Type::teeny, win::Color(0.8f, 0.8f, 0.8f, 1.0f), button.desc);

		const auto color = button.click ? buttonclicked : (mouseover ? buttonhover : buttoncolor);

		renderables.menu_renderables.emplace_back(MenuTexture::button, button.x, button.y, Button::width, Button::height, color);
		renderables.text_renderables.emplace_back(button.x + Button::width / 2.0f, button.y + 0.2f, true, TextRenderable::Type::smol, textcolor, button.text);
	}

	renderables.light_renderables.emplace_back(0, input.x, input.y, theme, 500.0f);

	// place title
	if (menu.title != NULL && strlen(menu.title) > 0)
		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, win::Color(0.65f, 0.65f, 0.65f, 1.0f), menu.title);
}

void MenuService::add(Menu *menu)
{
	menustack.emplace_back(menu);
}

void MenuService::set_theme(const win::Color<float> &c)
{
	theme = c;
}
