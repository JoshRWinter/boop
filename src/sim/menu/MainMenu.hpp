#pragma once

#include <vector>

#include <win/Win.hpp>

#include "../../render/Renderable.hpp"
#include "../../Input.hpp"

struct Button
{
	Button(float x, float y, float w, float h, const char *text)
		: x(x), y(y), w(w), h(h), text(text), click(false) {}

	float x, y, w, h;
	const char *text;
	bool click;
};

struct MainMenuResult
{
	enum class Result
	{
		play, join, quit
	};

	Result result;
	std::string ip;
};

class MainMenu
{
	WIN_NO_COPY_MOVE(MainMenu);

	static constexpr float button_width = 3.0f;
	static constexpr float button_height = 0.8f;

	inline static auto color = win::Color<float>(0.8f, 0.4f, 0.4f, 1.0f);
	inline static auto hover = win::Color<float>(0.6f, 0.2f, 0.2f, 1.0f);
	inline static auto clicked = win::Color<float>(0.4f, 0.1f, 0.1f, 1.0f);

public:
	MainMenu();

	void show(Renderables &renderables, const Input &input);

private:
	static MenuRenderable map_renderable(const Button &button, float x, float y);
	static TextRenderable map_text(const Button &button);
	static bool mouseover(const Button &button, float x, float y);
	static bool button_clicked(const Button &button, bool click, float x, float y);

	Button host, join, quit;
};