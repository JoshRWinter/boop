#include "GameModeMenu.hpp"

GameModeMenu::GameModeMenu(std::function<void(GameMode m)> result)
	: Menu("Select game mode")
	, result(std::move(result))
{
	buttons.emplace_back(-Button::width / 2.0f,
						 -0.75f,
						 "Ten",
						 "First to ten points wins",
						 [this]
						 {
							 this->result(GameMode::ten);
							 quit = true;
						 });

	buttons.emplace_back(-Button::width / 2.0f,
						 -1.75f,
						 "Twos",
						 "First to score 2\npoints in a row wins",
						 [this]
						 {
							 this->result(GameMode::twos);
							 quit = true;
						 });

	buttons.emplace_back(-Button::width / 2.0f,
						 -2.75f,
						 "Endless",
						 "Play foreverr",
						 [this]
						 {
							 this->result(GameMode::endless);
							 quit = true;
						 });

	buttons.emplace_back(-Button::width / 2.0f,
						 -4.0f,
						 "Back",
						 (const char *)NULL,
						 [this]
						 {
							 quit = true;
						 });
}
