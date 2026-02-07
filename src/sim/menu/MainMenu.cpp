#include <ctime>
#include <random>

#include "../../Colors.hpp"
#include "DifficultyMenu.hpp"
#include "GameModeMenu.hpp"
#include "HostMenu.hpp"
#include "JoinMenu.hpp"
#include "MainMenu.hpp"

MainMenu::MainMenu(MenuService &menus, NetworkMatch &match, std::function<void(Color c, DifficultyLevel d, GameMode g)> result, std::function<void()> exit)
	: Menu(NULL)
	, menus(menus)
	, match(match)
	, result(std::move(result))
	, exit(std::move(exit))
	, red(-3.0f, 0.8f, Color::red)
	, green(-2.0f, 0.8f, Color::green)
	, blue(-1.0f, 0.8f, Color::blue)
	, yellow(0.25f, 0.8f, Color::yellow)
	, cyan(1.25f, 0.8f, Color::cyan)
	, magenta(2.25f, 0.8f, Color::magenta)
{
	std::mt19937 mersenne(time(NULL));
	const auto i = std::uniform_int_distribution<int>(0, 5)(mersenne);
	colors[i]->selected = true;
	current = colors[i]->color;

	menus.set_theme(get_color(current));

	buttons.emplace_back(-Button::width / 2.0f,
						 -0.75f,
						 "Computer",
						 "Play against one\nof the ghosts inside\nyour computer",
						 [this]
						 {
							 this->menus.add(new GameModeMenu(
								 [this](GameMode m)
								 {
									 this->menus.add(new DifficultyMenu(
										 [this, m](DifficultyLevel d)
										 {
											 this->match.start_bot(d);
											 this->menus.add(new HostMenu(this->match,
																		  [this, m, d]()
																		  {
																			  this->result(current, d, m);
																		  }));
										 }));
								 }));
						 });

	buttons.emplace_back(-Button::width / 2.0f,
						 -1.75f,
						 "Host",
						 "Host a game",
						 [this]
						 {
							 this->menus.add(new GameModeMenu(
								 [this](GameMode m)
								 {
									 this->menus.add(new DifficultyMenu(
										 [this, m](DifficultyLevel d)
										 {
											 this->menus.add(new HostMenu(this->match,
																		  [this, d, m]()
																		  {
																			  this->result(current, d, m);
																		  }));
										 }));
								 }));
						 });

	buttons.emplace_back(-Button::width / 2.0f,
						 -2.75f,
						 "Join",
						 "Join a game",
						 [this]
						 {
							 this->menus.add(new JoinMenu(this->menus,
														  this->match,
														  [this]
														  {
															  this->result(current,
																		   DifficultyLevel::easy,
																		   GameMode::endless); // these values are ultimately ignored
														  }));
						 });

	buttons.emplace_back(-Button::width / 2.0f,
						 -4.0f,
						 "Quit",
						 ":(",
						 [this]
						 {
							 this->exit();
						 });

	tick = [this](Renderables &renderables, const Input &input, const std::vector<char> &text_renderables)
	{
		for (auto *color : colors)
		{
			if (input.x > color->x && input.x < color->x + ColorSelect::size && input.y > color->y && input.y < color->y + ColorSelect::size)
			{
				color->selected = true;
				this->current = color->color;
				this->menus.set_theme(get_color(current));

				for (auto *c2 : colors)
				{
					if (c2 == color)
						continue;

					c2->selected = false;
				}
			}

			auto c = get_color(color->color);
			c.alpha = 0.4f;
			renderables.menu_renderables.emplace_back(MenuTexture::colorselect,
													  color->x,
													  color->y + (color->selected ? 0.2f : 0.0f),
													  ColorSelect::size,
													  ColorSelect::size,
													  c);
		}

		if (error != NULL)
			renderables.text_renderables.emplace_back(-5.0f, -2.0f, true, TextRenderable::Type::teeny, get_color(Color::red), error);

		renderables.text_renderables.emplace_back(0.0f, 2.5f, true, TextRenderable::Type::yuge, get_color(current), "BOOP");

		if (input.x > 7.8f && input.x < 8.0f && input.y > -4.5f && input.y < -4.2f)
			renderables.text_renderables.emplace_back(6.75f, -4.45f, true, TextRenderable::Type::teeny, win::Color(0.4f, 0.4f, 0.4f, 1.0f), "Look behind you");
		else
			renderables.text_renderables.emplace_back(7.8f, -4.45f, false, TextRenderable::Type::teeny, win::Color(0.4f, 0.4f, 0.4f, 1.0f), "?");
	};
}
