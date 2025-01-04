#include "MainMenu.hpp"

MainMenu::MainMenu()
	: state(MainMenuState::main)
	, host(-button_width / 2.0f, -1.0f, button_width, button_height, "Host")
	, join(-button_width / 2.0f, -2.0f, button_width, button_height, "Join")
	, quit(-button_width / 2.0f, -3.0f, button_width, button_height, "Quit")
	, back(-button_width / 2.0f, -3.0f, button_width, button_height, "Back")
{}

MainMenuResult MainMenu::show(Renderables &renderables, const Input &input, NetworkMatch &match)
{
	switch (state)
	{
		case MainMenuState::main:
			return show_main(renderables, input, match);
		case MainMenuState::host:
			return show_host(renderables, input, match);
		case MainMenuState::join:
			return show_join(renderables, input, match);
		case MainMenuState::joining:
			return show_joining(renderables, input, match);

		default: win::bug("wut");
	}
}

MainMenuResult MainMenu::show_main(Renderables &renderables, const Input &input, NetworkMatch &match)
{
	if (button_clicked(host, input.click, input.x, input.y))
	{
		host.click = false;
		state = MainMenuState::host;
	}
	else if (button_clicked(join, input.click, input.x, input.y))
	{
		join.click = false;
		state = MainMenuState::join;
	}
	else if (button_clicked(quit, input.click, input.x, input.y))
	{
		quit.click = false;
		return MainMenuResult::quit;
	}

	host.click = mouseover(host, input.x, input.y) && input.click;
	join.click = mouseover(join, input.x, input.y) && input.click;
	quit.click = mouseover(quit, input.x, input.y) && input.click;

	renderables.menu_renderables.emplace_back(map_renderable(host, input.x, input.y));
	renderables.menu_renderables.emplace_back(map_renderable(join, input.x, input.y));
	renderables.menu_renderables.emplace_back(map_renderable(quit, input.x, input.y));

	renderables.text_renderables.emplace_back(map_text(host));
	renderables.text_renderables.emplace_back(map_text(join));
	renderables.text_renderables.emplace_back(map_text(quit));

	renderables.text_renderables.emplace_back(0, 0.0f, 2.0f, true, TextRenderable::Type::yuge, win::Color(1.0f, 0.0f, 0.0f, 1.0f), "boop");

	return MainMenuResult::none;
}

MainMenuResult MainMenu::show_host(Renderables &renderables, const Input &input, NetworkMatch &match)
{
	bool try_host = true;
	if (button_clicked(back, input.click, input.x, input.y))
	{
		back.click = false;
		state = MainMenuState::main;
		match.reset();
		try_host = false;
	}

	back.click = mouseover(back, input.x, input.y) && input.click;

	renderables.menu_renderables.emplace_back(map_renderable(back, input.x, input.y));
	renderables.text_renderables.emplace_back(map_text(back));

	renderables.text_renderables.emplace_back(0, 0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Hosting on port 28857");

	if (try_host && match.host())
		return MainMenuResult::play;

	return MainMenuResult::none;
}

MainMenuResult MainMenu::show_join(Renderables &renderables, const Input &input, NetworkMatch &match)
{
	if (button_clicked(back, input.click, input.x, input.y))
	{
		back.click = false;
		state = MainMenuState::main;
	}
	else if (button_clicked(join, input.click, input.x, input.y))
	{
		join.click = false;
		state = MainMenuState::joining;
	}

	back.click = mouseover(back, input.x, input.y) && input.click;
	join.click = mouseover(join, input.x, input.y) && input.click;

	renderables.menu_renderables.emplace_back(map_renderable(back, input.x, input.y));
	renderables.menu_renderables.emplace_back(map_renderable(join, input.x, input.y));

	renderables.text_renderables.emplace_back(map_text(back));
	renderables.text_renderables.emplace_back(map_text(join));

	renderables.text_renderables.emplace_back(0, 0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Type an IP Address");
	renderables.text_renderables.emplace_back(0, 0.0f, 1.0f, true, TextRenderable::Type::smol, textcolor, ip_input.c_str());

	return MainMenuResult::none;
}

MainMenuResult MainMenu::show_joining(Renderables &renderables, const Input &input, NetworkMatch &match)
{
	bool try_join = true;
	if (button_clicked(back, input.click, input.x, input.y))
	{
		back.click = false;
		state = MainMenuState::main;
		match.reset();
		try_join = false;
	}

	back.click = mouseover(back, input.x, input.y) && input.click;

	renderables.menu_renderables.emplace_back(map_renderable(back, input.x, input.y));
	renderables.text_renderables.emplace_back(map_text(back));

	renderables.text_renderables.emplace_back(0, 0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Joining...");

	if (try_join && match.join(ip_input.c_str()))
		return MainMenuResult::play;

	return MainMenuResult::none;
}

MenuRenderable MainMenu::map_renderable(const Button &button, float x, float y)
{
	return MenuRenderable(0, MenuTexture::button, button.x, button.y, button.w, button.h, button.click ? clicked : (mouseover(button, x, y) ? hover : color));
}

TextRenderable MainMenu::map_text(const Button &button)
{
	return TextRenderable(1, button.x + button.w / 2.0f, button.y + 0.2f, true, TextRenderable::Type::smol, win::Color<float>(1.0f, 0.5f, 0.5f, 1.0f), button.text);
}

bool MainMenu::mouseover(const Button &button, float x, float y)
{
	return x >= button.x && x <= button.x + button.w && y >= button.y && y <= button.y + button.h;
}

bool MainMenu::button_clicked(const Button &button, bool click, float x, float y)
{
	return button.click && !click && mouseover(button, x, y);
}
