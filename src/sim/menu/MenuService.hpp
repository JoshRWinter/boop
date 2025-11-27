#pragma once

#include <memory>
#include <vector>

#include <win/Win.hpp>

#include "../../Input.hpp"
#include "../../render/Renderable.hpp"
#include "../SoundManager.hpp"
#include "Menu.hpp"

class MenuService
{
	WIN_NO_COPY_MOVE(MenuService);

	inline static auto textcolor = win::Color<float>(0.6f, 0.6f, 0.6f, 0.8f);
	inline static auto buttoncolor = win::Color<float>(0.075f, 0.075f, 0.075f, 0.2f);
	inline static auto buttonhover = win::Color<float>(0.4f, 0.05f, 0.05f, 0.5f);
	inline static auto buttonclicked = win::Color<float>(0.3f, 0.02f, 0.02f, 0.6f);

public:
	explicit MenuService(SoundManager &sounds);
	void run(Renderables &renderables, const Input &input, const std::vector<char> &text_input);

	void add(Menu *menu);
	void set_theme(const win::Color<float> &c);

private:
	SoundManager &sounds;
	std::vector<std::unique_ptr<Menu>> menustack;
	win::Color<float> theme;
};
