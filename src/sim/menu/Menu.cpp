#include <random>
#include <time.h>

#include "Menu.hpp"

MainMenuResult Menu::menu_main(SimulationHost &host, NetworkMatch &match, const char *errmsg)
{
	ColorSelect red(-3.0f, 0.8f, 0.75f, 0.75f, get_red());
	ColorSelect green(-2.0f, 0.8f, 0.75f, 0.75f, get_green());
	ColorSelect blue(-1.0f, 0.8f, 0.75f, 0.75f, get_blue());
	ColorSelect yellow(0.25f, 0.8f, 0.75f, 0.75f, get_yellow());
	ColorSelect cyan(1.25f, 0.8f, 0.75f, 0.75f, get_cyan());
	ColorSelect magenta(2.25f, 0.8f, 0.75f, 0.75f, get_magenta());

	ColorSelect *colors[] = {&red, &green, &blue, &yellow, &cyan, &magenta};
	std::mt19937 mersenne(time(NULL));
	auto color = (Color) std::uniform_int_distribution<int>(0, 5)(mersenne);

	Button computer(-button_width / 2.0f, -0.75f, button_width, button_height, "Computer");
	Button hostgame(-button_width / 2.0f, -1.75f, button_width, button_height, "Host");
	Button join(-button_width / 2.0f, -2.75f, button_width, button_height, "Join");
	Button quit(-button_width / 2.0f, -3.75f, button_width, button_height, "Quit");

	while (!host.quit())
	{
		const auto input = host.get_input();

		if (button_clicked(computer, input.click, input.x, input.y))
		{
			computer.click = false;

			DifficultyLevel diff;
			if (menu_choose_difficulty(host, diff))
			{
				match.start_bot(diff);
				auto error = NetworkMatch::ErrorReason::none;
				if (menu_host(host, match, error))
					return MainMenuResult(color, diff);

				if (error != NetworkMatch::ErrorReason::none)
					errmsg = "Hosting error";
			}
		}
		else if (button_clicked(hostgame, input.click, input.x, input.y))
		{
			hostgame.click = false;

			DifficultyLevel diff;
			if (menu_choose_difficulty(host, diff))
			{
				auto error = NetworkMatch::ErrorReason::none;
				if (menu_host(host, match, error))
					return MainMenuResult(color, diff);

				if (error != NetworkMatch::ErrorReason::none)
					errmsg = "Hosting error";
			}
		}
		else if (button_clicked(join, input.click, input.x, input.y))
		{
			join.click = false;
			auto error = NetworkMatch::ErrorReason::none;
			if (menu_join(host, match, error))
				return MainMenuResult(color, DifficultyLevel::easy);

			if (error != NetworkMatch::ErrorReason::none)
				errmsg = "Joining error";
		}
		else if (button_clicked(quit, input.click, input.x, input.y))
		{
			quit.click = false;
		}

		computer.click = mouseover(computer, input.x, input.y) && input.click;
		hostgame.click = mouseover(hostgame, input.x, input.y) && input.click;
		join.click = mouseover(join, input.x, input.y) && input.click;
		quit.click = mouseover(quit, input.x, input.y) && input.click;

		auto &renderables = host.get_renderables();

		map_renderables(renderables, computer, input.x, input.y);
		map_renderables(renderables, hostgame, input.x, input.y);
		map_renderables(renderables, join, input.x, input.y);
		map_renderables(renderables, quit, input.x, input.y);

		for (int i = 0; i < 6; ++i)
		{
			if (mouseover(*colors[i], input.x, input.y))
				color = (Color) i;

			map_renderables(renderables, *colors[i], (int) color == i);
		}

		renderables.text_renderables.emplace_back(0.0f, 2.5f, true, TextRenderable::Type::yuge, colors[(int) color]->color, "boop");

		if (errmsg != NULL)
			renderables.text_renderables.emplace_back(0.0f, 3.8f, true, TextRenderable::Type::smol, win::Color<float>(0.8f, 0.1f, 0.1f, 1.0f), errmsg);

		host.release_renderables(renderables);
		host.sleep();
	}

	return MainMenuResult(color, DifficultyLevel::easy);
}

bool Menu::menu_choose_difficulty(SimulationHost &host, DifficultyLevel &level)
{
	Button easy(-button_width / 2.0f, 0.0f, button_width, button_height, "Ezpz");
	Button medium(-button_width / 2.0f, -1.0f, button_width, button_height, "Medium");
	Button hard(-button_width / 2.0f, -2.0f, button_width, button_height, "Hard");

	Button back(-button_width / 2.0f, -3.5f, button_width, button_height, "Back");

	while (true)
	{
		if (host.quit())
			return false;

		const auto input = host.get_input();

		if (button_clicked(easy, input.click, input.x, input.y))
		{
			level = DifficultyLevel::easy;
			return true;
		}

		if (button_clicked(medium, input.click, input.x, input.y))
		{
			level = DifficultyLevel::med;
			return true;
		}

		if (button_clicked(hard, input.click, input.x, input.y))
		{
			level = DifficultyLevel::hard;
			return true;
		}

		if (button_clicked(back, input.click, input.x, input.y))
		{
			return false;
		}

		easy.click = mouseover(easy, input.x, input.y) && input.click;
		medium.click = mouseover(medium, input.x, input.y) && input.click;
		hard.click = mouseover(hard, input.x, input.y) && input.click;
		back.click = mouseover(back, input.x, input.y) && input.click;

		auto &renderables = host.get_renderables();

		map_renderables(renderables, easy, input.x, input.y);
		map_renderables(renderables, medium, input.x, input.y);
		map_renderables(renderables, hard, input.x, input.y);
		map_renderables(renderables, back, input.x, input.y);
		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Choose difficulty");
		host.release_renderables(renderables);

		host.sleep();
	}
}

bool Menu::menu_host(SimulationHost &host, NetworkMatch &match, NetworkMatch::ErrorReason &error)
{
	Button back(-button_width / 2.0f, -3.5f, button_width, button_height, "Back");

	while (!host.quit())
	{
		const auto input = host.get_input();

		if (button_clicked(back, input.click, input.x, input.y))
		{
			error = NetworkMatch::ErrorReason::none;
			back.click = false;
			match.reset();
			return false;
		}

		back.click = mouseover(back, input.x, input.y) && input.click;

		if (match.host())
			return true;
		else
		{
			error = match.errored();
			if (error != NetworkMatch::ErrorReason::none)
				return false;
		}

		auto &renderables = host.get_renderables();
		map_renderables(renderables, back, input.x, input.y);
		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Hosting on port 28857");
		host.release_renderables(renderables);

		host.sleep();
	}

	return false;
}

bool Menu::menu_join(SimulationHost &host, NetworkMatch &match, NetworkMatch::ErrorReason &error)
{
	Button join(-button_width / 2.0f, -2.5f, button_width, button_height, "Join");
	Button back(-button_width / 2.0f, -3.5f, button_width, button_height, "Back");

	std::vector<char> text;
	std::string ip_input;

	while (!host.quit())
	{
		const auto &input = host.get_input();
		text.clear();
		host.get_text_input(text);

		for (auto c : text)
		{
			if (c == '\b')
			{
				if (!ip_input.empty())
					ip_input.pop_back();
			}
			else
				ip_input.push_back(c);
		}

		if (button_clicked(back, input.click, input.x, input.y))
		{
			back.click = false;
			match.reset();
			error = NetworkMatch::ErrorReason::none;
			return false;
		}
		else if (button_clicked(join, input.click, input.x, input.y))
		{
			join.click = false;
			error = NetworkMatch::ErrorReason::none;
			return menu_joining(host, match, error, ip_input.c_str());
		}

		back.click = mouseover(back, input.x, input.y) && input.click;
		join.click = mouseover(join, input.x, input.y) && input.click;

		auto &renderables = host.get_renderables();
		map_renderables(renderables, back, input.x, input.y);
		map_renderables(renderables, join, input.x, input.y);

		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Type an IP Address");
		renderables.text_renderables.emplace_back(0.0f, 1.0f, true, TextRenderable::Type::smol, textcolor, ip_input.c_str());

		host.release_renderables(renderables);
		host.sleep();
	}

	return false;
}

bool Menu::menu_joining(SimulationHost &host, NetworkMatch &match, NetworkMatch::ErrorReason &error, const char *ip)
{
	Button back(-button_width / 2.0f, -3.5f, button_width, button_height, "Back");

	while (!host.quit())
	{
		const auto &input = host.get_input();

		if (button_clicked(back, input.click, input.x, input.y))
		{
			back.click = false;
			match.reset();
			return false;
		}
		back.click = mouseover(back, input.x, input.y) && input.click;

		if (match.join(ip))
			return true;
		else
		{
			error = match.errored();
			if (error != NetworkMatch::ErrorReason::none)
			{
				match.reset();
				return false;
			}
		}

		auto &renderables = host.get_renderables();
		map_renderables(renderables, back, input.x, input.y);
		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Joining...");
		host.release_renderables(renderables);

		host.sleep();
	}

	return false;
}

void Menu::map_renderables(Renderables &renderables, const Button &button, float x, float y)
{
	renderables.menu_renderables.emplace_back(MenuTexture::button, button.x, button.y, button.w, button.h, button.click ? clicked : (mouseover(button, x, y) ? hover : color));
	renderables.text_renderables.emplace_back(button.x + button.w / 2.0f, button.y + 0.2f, true, TextRenderable::Type::smol, win::Color<float>(1.0f, 0.5f, 0.5f, 1.0f), button.text);
}

void Menu::map_renderables(Renderables &renderables, const ColorSelect &color, bool selected)
{
	const float raise = 0.2f;

	renderables.menu_renderables.emplace_back(MenuTexture::colorselect, color.x, color.y + (selected ? raise : 0.0f), color.w, color.h, color.color);
}

bool Menu::mouseover(const Button &button, float x, float y)
{
	return x >= button.x && x <= button.x + button.w && y >= button.y && y <= button.y + button.h;
}

bool Menu::mouseover(const ColorSelect &color, float x, float y)
{
	return x >= color.x && x <= color.x + color.w && y >= color.y && y <= color.y + color.h;
}

bool Menu::button_clicked(const Button &button, bool click, float x, float y)
{
	return button.click && !click && mouseover(button, x, y);
}
