#pragma once

#include <vector>

#include <win/Win.hpp>

#include "../../render/Renderable.hpp"
#include "../../Input.hpp"
#include "../NetworkMatch.hpp"

struct Button
{
	Button(float x, float y, float w, float h, const char *text)
		: x(x), y(y), w(w), h(h), text(text), click(false) {}

	float x, y, w, h;
	const char *text;
	bool click;
};

enum class MainMenuResult
{
	none,
	play,
	quit
};

enum class MainMenuState
{
	main,
	host,
	join,
	joining
};

class MainMenu
{
	WIN_NO_COPY_MOVE(MainMenu);

	static constexpr float button_width = 3.0f;
	static constexpr float button_height = 0.8f;

	inline static auto textcolor = win::Color<float>(0.8f, 0.6f, 0.6f, 1.0f);

	inline static auto color = win::Color<float>(0.8f, 0.4f, 0.4f, 1.0f);
	inline static auto hover = win::Color<float>(0.6f, 0.2f, 0.2f, 1.0f);
	inline static auto clicked = win::Color<float>(0.4f, 0.1f, 0.1f, 1.0f);

public:
	MainMenu();

	MainMenuResult show(Renderables &renderables, const Input &input, NetworkMatch &match);

private:
	MainMenuResult show_main(Renderables &renderables, const Input &input, NetworkMatch &match);
	MainMenuResult show_host(Renderables &renderables, const Input &input, NetworkMatch &match);
	MainMenuResult show_join(Renderables &renderables, const Input &input, NetworkMatch &match);
	MainMenuResult show_joining(Renderables &renderables, const Input &input, NetworkMatch &match);

	static MenuRenderable map_renderable(const Button &button, float x, float y);
	static TextRenderable map_text(const Button &button);
	static bool mouseover(const Button &button, float x, float y);
	static bool button_clicked(const Button &button, bool click, float x, float y);

	MainMenuState state;
	Button computer, host, join, quit, back;
	std::string ip_input = "::1";
};
