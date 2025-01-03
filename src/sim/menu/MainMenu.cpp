#include "MainMenu.hpp"

MainMenu::MainMenu()
	: host(-button_width / 2.0f, -1.0f, button_width, button_height, "Host")
	, join(-button_width / 2.0f, -2.0f, button_width, button_height, "Join")
	, quit(-button_width / 2.0f, -3.0f, button_width, button_height, "Quit")
{}

void MainMenu::show(Renderables &renderables, const Input &input)
{
	if (button_clicked(host, input.click, input.x, input.y))
	{
		fprintf(stderr, "host clicked\n");
	}
	else if (button_clicked(join, input.click, input.x, input.y))
	{
		fprintf(stderr, "join clicked\n");
	}
	else if (button_clicked(quit, input.click, input.x, input.y))
	{
		fprintf(stderr, "quit clicked\n");
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
