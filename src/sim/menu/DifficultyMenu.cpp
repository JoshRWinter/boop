#include "DifficultyMenu.hpp"

DifficultyMenu::DifficultyMenu(std::function<void(DifficultyLevel d)> result)
	: Menu("Select difficulty")
	, result(std::move(result))
{
	buttons.emplace_back(-Button::width / 2.0f,
						 -0.75f,
						 "EZ",
						 (const char *)NULL,
						 [this]
						 {
							 this->result(DifficultyLevel::easy);
							 quit = true;
						 });

	buttons.emplace_back(-Button::width / 2.0f,
						 -1.75f,
						 "Medium",
						 (const char *)NULL,
						 [this]
						 {
							 this->result(DifficultyLevel::med);
							 quit = true;
						 });

	buttons.emplace_back(-Button::width / 2.0f,
						 -2.75f,
						 "Hard",
						 (const char *)NULL,
						 [this]
						 {
							 this->result(DifficultyLevel::hard);
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
