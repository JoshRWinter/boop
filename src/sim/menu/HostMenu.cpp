#include "HostMenu.hpp"

HostMenu::HostMenu(NetworkMatch &match, std::function<void()> result)
	: Menu("Listening for connections...")
	, result(std::move(result))
	, match(match)
{
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
		error = this->match.errored();

		if (error != NetworkMatch::ErrorReason::none)
		{
			renderables.text_renderables.emplace_back(0.0f,
													  0.0f,
													  true,
													  TextRenderable::Type::smol,
													  win::Color(1.0f, 0.0f, 0.0f, 1.0f),
													  NetworkMatch::error_reason(error));
		}
		else
		{
			if (this->match.host())
			{
				this->result();
				quit = true;
			}
		}
	};
}
