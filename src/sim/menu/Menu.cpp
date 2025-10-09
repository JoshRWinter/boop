#include <random>
#include <time.h>

#include "Menu.hpp"

MainMenuResult Menu::menu_main(SimulationHost &host, SoundManager &sounds, NetworkMatch &match, const win::Area<float> &area, const char *errmsg)
{
	ColorSelect red(-3.0f, 0.8f, 0.75f, 0.75f, get_red());
	ColorSelect green(-2.0f, 0.8f, 0.75f, 0.75f, get_green());
	ColorSelect blue(-1.0f, 0.8f, 0.75f, 0.75f, get_blue());
	ColorSelect yellow(0.25f, 0.8f, 0.75f, 0.75f, get_yellow());
	ColorSelect cyan(1.25f, 0.8f, 0.75f, 0.75f, get_cyan());
	ColorSelect magenta(2.25f, 0.8f, 0.75f, 0.75f, get_magenta());

	ColorSelect *colors[] = {&red, &green, &blue, &yellow, &cyan, &magenta};
	std::mt19937 mersenne(time(NULL));
	auto color = (Color)std::uniform_int_distribution<int>(0, 5)(mersenne);

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

			sounds.play_menu_click();

			GameMode mode;
			if (menu_choose_gamemode(host, sounds, area, color, mode))
			{
				DifficultyLevel diff;
				if (menu_choose_difficulty(host, sounds, area, color, diff))
				{
					match.start_bot(diff);
					auto error = NetworkMatch::ErrorReason::none;
					if (menu_host(host, sounds, match, error, area, color))
						return MainMenuResult(color, diff, mode);

					if (error != NetworkMatch::ErrorReason::none)
						errmsg = "Hosting error";
				}
			}
		}
		else if (button_clicked(hostgame, input.click, input.x, input.y))
		{
			hostgame.click = false;

			sounds.play_menu_click();

			GameMode mode;
			if (menu_choose_gamemode(host, sounds, area, color, mode))
			{
				DifficultyLevel diff;
				if (menu_choose_difficulty(host, sounds, area, color, diff))
				{
					auto error = NetworkMatch::ErrorReason::none;
					if (menu_host(host, sounds, match, error, area, color))
						return MainMenuResult(color, diff, mode);

					if (error != NetworkMatch::ErrorReason::none)
						errmsg = "Hosting error";
				}
			}
		}
		else if (button_clicked(join, input.click, input.x, input.y))
		{
			join.click = false;

			sounds.play_menu_click();

			auto error = NetworkMatch::ErrorReason::none;
			if (menu_join(host, sounds, match, error, area, color))
				// the difficulty and mode settings here are ignored, since the host actually controls that, and we are the guest
				return MainMenuResult(color, DifficultyLevel::easy, GameMode::endless);

			if (error != NetworkMatch::ErrorReason::none)
				errmsg = "Joining error";
		}
		else if (button_clicked(quit, input.click, input.x, input.y))
		{
			quit.click = false;

			sounds.play_menu_click();
		}

		computer.click = mouseover(computer, input.x, input.y) && input.click;
		hostgame.click = mouseover(hostgame, input.x, input.y) && input.click;
		join.click = mouseover(join, input.x, input.y) && input.click;
		quit.click = mouseover(quit, input.x, input.y) && input.click;

		auto &renderables = host.get_renderables();

		map_theme(renderables, area, input, color);

		map_renderables(renderables, computer, input.x, input.y);
		map_renderables(renderables, hostgame, input.x, input.y);
		map_renderables(renderables, join, input.x, input.y);
		map_renderables(renderables, quit, input.x, input.y);

		for (int i = 0; i < 6; ++i)
		{
			if (mouseover(*colors[i], input.x, input.y))
				color = (Color)i;

			map_renderables(renderables, *colors[i], (int)color == i);
		}

		renderables.text_renderables.emplace_back(0.0f, 2.5f, true, TextRenderable::Type::yuge, colors[(int)color]->color, "boop");

		if (errmsg != NULL)
			renderables.text_renderables.emplace_back(0.0f, 3.8f, true, TextRenderable::Type::smol, win::Color<float>(0.8f, 0.1f, 0.1f, 1.0f), errmsg);

		host.release_renderables_and_sleep(renderables);
	}

	// the difficulty and mode settings here are ignored, because if we are in this spot, the game is trying to exit and the function just needs to return _something_
	return MainMenuResult(color, DifficultyLevel::easy, GameMode::endless);
}

bool Menu::menu_choose_difficulty(SimulationHost &host, SoundManager &sounds, const win::Area<float> &area, Color color, DifficultyLevel &level)
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
			easy.click = false;

			sounds.play_menu_click();

			level = DifficultyLevel::easy;
			return true;
		}

		if (button_clicked(medium, input.click, input.x, input.y))
		{
			medium.click = false;

			sounds.play_menu_click();

			level = DifficultyLevel::med;
			return true;
		}

		if (button_clicked(hard, input.click, input.x, input.y))
		{
			hard.click = false;

			sounds.play_menu_click();

			level = DifficultyLevel::hard;
			return true;
		}

		if (button_clicked(back, input.click, input.x, input.y))
		{
			back.click = false;

			sounds.play_menu_click();

			return false;
		}

		easy.click = mouseover(easy, input.x, input.y) && input.click;
		medium.click = mouseover(medium, input.x, input.y) && input.click;
		hard.click = mouseover(hard, input.x, input.y) && input.click;
		back.click = mouseover(back, input.x, input.y) && input.click;

		auto &renderables = host.get_renderables();

		map_theme(renderables, area, input, color);

		map_renderables(renderables, easy, input.x, input.y);
		map_renderables(renderables, medium, input.x, input.y);
		map_renderables(renderables, hard, input.x, input.y);
		map_renderables(renderables, back, input.x, input.y);
		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Choose difficulty");

		host.release_renderables_and_sleep(renderables);
	}
}

bool Menu::menu_choose_gamemode(SimulationHost &host, SoundManager &sounds, const win::Area<float> &area, Color color, GameMode &mode)
{
	Button ten(-button_width / 2.0f, 0.0f, button_width, button_height, "Ten");
	Button twos(-button_width / 2.0f, -1.0f, button_width, button_height, "Twos");
	Button endless(-button_width / 2.0f, -2.0f, button_width, button_height, "Endless");

	Button back(-button_width / 2.0f, -3.5f, button_width, button_height, "Back");

	while (true)
	{
		if (host.quit())
			return false;

		const auto input = host.get_input();

		if (button_clicked(ten, input.click, input.x, input.y))
		{
			ten.click = false;

			sounds.play_menu_click();

			mode = GameMode::ten;
			return true;
		}

		if (button_clicked(twos, input.click, input.x, input.y))
		{
			twos.click = false;

			sounds.play_menu_click();

			mode = GameMode::twos;
			return true;
		}

		if (button_clicked(endless, input.click, input.x, input.y))
		{
			endless.click = false;

			sounds.play_menu_click();

			mode = GameMode::endless;
			return true;
		}

		if (button_clicked(back, input.click, input.x, input.y))
		{
			back.click = false;

			sounds.play_menu_click();

			return false;
		}

		ten.click = mouseover(ten, input.x, input.y) && input.click;
		twos.click = mouseover(twos, input.x, input.y) && input.click;
		endless.click = mouseover(endless, input.x, input.y) && input.click;
		back.click = mouseover(back, input.x, input.y) && input.click;

		auto &renderables = host.get_renderables();

		map_theme(renderables, area, input, color);

		map_renderables(renderables, ten, input.x, input.y);
		map_renderables(renderables, twos, input.x, input.y);
		map_renderables(renderables, endless, input.x, input.y);
		map_renderables(renderables, back, input.x, input.y);
		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Choose game mode");

		const char *description = NULL;

		if (mouseover(ten, input.x, input.y))
			description = "First to 10 points wins";
		else if (mouseover(twos, input.x, input.y))
			description = "First to score 2 points\nin a row wins";
		else if (mouseover(endless, input.x, input.y))
			description = "Play foreverr";
		else if (mouseover(back, input.x, input.y))
			description = "Be a party pooper and don't play";

		if (description != NULL)
			renderables.text_renderables.emplace_back(5.0f, -1.0f, true, TextRenderable::Type::teeny, textcolor, description);

		host.release_renderables_and_sleep(renderables);
	}
}

bool Menu::menu_host(SimulationHost &host, SoundManager &sounds, NetworkMatch &match, NetworkMatch::ErrorReason &error, const win::Area<float> &area, Color color)
{
	Button back(-button_width / 2.0f, -3.5f, button_width, button_height, "Back");

	while (!host.quit())
	{
		const auto input = host.get_input();

		if (button_clicked(back, input.click, input.x, input.y))
		{
			back.click = false;

			sounds.play_menu_click();

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

		map_theme(renderables, area, input, color);

		map_renderables(renderables, back, input.x, input.y);
		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Hosting on port 28857");
		host.release_renderables_and_sleep(renderables);
	}

	return false;
}

bool Menu::menu_join(SimulationHost &host, SoundManager &sounds, NetworkMatch &match, NetworkMatch::ErrorReason &error, const win::Area<float> &area, Color color)
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

			sounds.play_menu_click();

			match.reset();
			error = NetworkMatch::ErrorReason::none;
			return false;
		}
		else if (button_clicked(join, input.click, input.x, input.y))
		{
			join.click = false;

			sounds.play_menu_click();

			error = NetworkMatch::ErrorReason::none;
			return menu_joining(host, sounds, match, error, area, color, ip_input.c_str());
		}

		back.click = mouseover(back, input.x, input.y) && input.click;
		join.click = mouseover(join, input.x, input.y) && input.click;

		auto &renderables = host.get_renderables();

		map_theme(renderables, area, input, color);

		map_renderables(renderables, back, input.x, input.y);
		map_renderables(renderables, join, input.x, input.y);

		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Type an IP Address");
		renderables.text_renderables.emplace_back(0.0f, 1.0f, true, TextRenderable::Type::smol, textcolor, ip_input.c_str());

		host.release_renderables_and_sleep(renderables);
	}

	return false;
}

bool Menu::menu_joining(SimulationHost &host, SoundManager &sounds, NetworkMatch &match, NetworkMatch::ErrorReason &error, const win::Area<float> &area, Color color, const char *ip)
{
	Button back(-button_width / 2.0f, -3.5f, button_width, button_height, "Back");

	while (!host.quit())
	{
		const auto &input = host.get_input();

		if (button_clicked(back, input.click, input.x, input.y))
		{
			back.click = false;

			sounds.play_menu_click();

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

		map_theme(renderables, area, input, color);

		map_renderables(renderables, back, input.x, input.y);
		renderables.text_renderables.emplace_back(0.0f, 2.0f, true, TextRenderable::Type::smol, textcolor, "Joining...");
		host.release_renderables_and_sleep(renderables);
	}

	return false;
}

void Menu::map_renderables(Renderables &renderables, const Button &button, float x, float y)
{
	renderables.menu_renderables.emplace_back(MenuTexture::button, button.x, button.y, button.w, button.h, button.click ? clicked : (mouseover(button, x, y) ? hover : color));
	renderables.text_renderables.emplace_back(button.x + button.w / 2.0f, button.y + 0.2f, true, TextRenderable::Type::smol, textcolor, button.text);
}

void Menu::map_theme(Renderables &renderables, const win::Area<float> &area, const Input &input, Color color)
{
	renderables.renderables.emplace_back(
		0,
		Texture::background,
		area.left,
		area.bottom,
		area.right - area.left,
		area.top - area.bottom,
		0.0f,
		0.0f,
		win::Color<float>(1.0f, 1.0f, 1.0f, 1.0f),
		win::Color<float>(0.0f, 0.0f, 0.0f, 0.0f));

	renderables.light_renderables.emplace_back(0, input.x, input.y, get_color(color), 200.0f);
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
