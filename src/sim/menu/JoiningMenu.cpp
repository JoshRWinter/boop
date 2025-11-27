#include "JoiningMenu.hpp"

JoiningMenu::JoiningMenu(NetworkMatch &match, const char *ip, std::function<void()> result)
	: Menu("Joining...")
	, match(match)
	, ip(ip)
	, result(std::move(result))
{
	buttons.emplace_back(-Button::width / 2.0f,
						 -4.0f,
						 "Cancel",
						 (const char *)NULL,
						 [this]
						 {
							 quit = true;
						 });

	tick = [this](Renderables &renderables, const Input &input, const std::vector<char> &text_input)
	{
		const auto error = this->match.errored();
		if (error == NetworkMatch::ErrorReason::none)
		{
			if (this->match.join(this->ip))
			{
				this->result();
				this->quit = true;
			}
		}
		else
		{
			const auto msg = NetworkMatch::error_reason(error);
			renderables.text_renderables.emplace_back(0.0f, 0.0f, true, TextRenderable::Type::smol, win::Color(1.0f, 0.0f, 0.0f, 1.0f), msg);
		}
	};
}
