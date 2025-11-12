#include <cmath>

#include <win/Display.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>
#include <win/SimStateExchanger.hpp>

#include "render/Renderer.hpp"
#include "sim/Simulation.hpp"

#ifdef ROLLDATA
const static unsigned char rolldata[] =
{
	#include ROLLDATA
};
#endif

#if defined WINPLAT_WINDOWS && NDEBUG
int WinMain(HINSTANCE hinstance, HINSTANCE prev, PSTR cmd, int show)
#else
int main(int argc, char **argv)
#endif
{
#ifdef ROLLDATA
	win::AssetRoll roll(rolldata, sizeof(rolldata));
#else
	win::AssetRoll roll("boop.roll");
#endif

	// display setup
	win::DisplayOptions display_options;
#ifndef NDEBUG
	display_options.caption = "debug_window";
	display_options.fullscreen = false;
	display_options.width = 1600;
	display_options.height = 900;
	display_options.debug = false;
#else
	display_options.caption = "Boop";
	display_options.fullscreen = true;
	display_options.width = 1600;
	display_options.height = 900;
#endif
	display_options.gl_major = 4;
	display_options.gl_minor = 4;

	win::Display display(display_options);
	display.vsync(true);
	//display.cursor(false);
	bool fullscreen = display_options.fullscreen;

	win::load_gl_functions();

	win::Dimensions<int> screenres(display.width(), display.height());

	const float rightside = (screenres.width / (float)screenres.height) * 4.5f;
	win::Area<float> renderarea(-rightside, rightside, -4.5f, 4.5f);
	win::Area<float> gamearea(-8.0f, 8.0f, -4.5f, 4.5f);

	win::SimStateExchanger<Renderables> simexchanger(60);

	Simulation sim(&roll, gamearea, false, DifficultyLevel::easy, simexchanger);
	Renderer renderer(roll, screenres, renderarea);

	Input input;
	bool input_available = false;

	std::vector<char> text_buffer;
	bool text_available = false;

	bool quit = false;
	display.register_button_handler([
		&quit,
		&input_available,
		&input,
		&text_buffer,
		&text_available,
		&display,
		&fullscreen
	](win::Button button, bool press)
	{
		switch (button)
		{
			case win::Button::esc:
				if (press)
					quit = true;
				break;
			case win::Button::mouse_left:
				input.click = press;
				input_available = true;
				break;
			case win::Button::backspace:
				if (press)
				{
					text_buffer.push_back('\b');
					text_available = true;
				}
				break;
			case win::Button::f11:
				if (press)
				{
					fullscreen = !fullscreen;
					display.set_fullscreen(fullscreen);
				}
				break;
			default: break;
		}
	});

	display.register_window_handler([&quit](win::WindowEvent e)
	{
		if (e == win::WindowEvent::close)
			quit = true;
	});

	bool resize = false;
	std::chrono::time_point<std::chrono::high_resolution_clock> last_resize;
	display.register_resize_handler([&resize, &last_resize](int w, int h)
	{
		resize = true;
		last_resize = std::chrono::high_resolution_clock::now();
	});

	display.register_character_handler([&text_buffer, &text_available](int c)
	{
		if (c >= ' ' && c <= '~')
		{
			text_buffer.push_back(c);
			text_available = true;
		}
	});

	display.register_mouse_handler([&input, &input_available, &renderarea, &screenres](int x, int y)
	{
		input.x = ((x / (float)screenres.width) * (renderarea.right - renderarea.left)) + renderarea.left;
		input.y = -(((y / (float)screenres.height) * (renderarea.top - renderarea.bottom)) + renderarea.bottom);
		input_available = true;
	});

	bool cursor_enabled = true;

	while (!quit && !sim.should_quit())
	{
		if (resize && std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - last_resize).count() > 0.3f)
		{
			resize = false;

			screenres.width = display.width();
			screenres.height = display.height();

			const auto adjustment = (screenres.width / (float)screenres.height) * (renderarea.top - renderarea.bottom);
			renderarea.right = adjustment / 2.0f;
			renderarea.left = -adjustment / 2.0f;
			renderer.set_resolution(screenres, renderarea, roll);
		}

		display.process();

		if (input_available)
		{
			input_available = false;
			sim.set_input(input);
		}

		if (text_available)
		{
			text_available = false;
			sim.set_text_input(text_buffer);
			text_buffer.clear();
		}

		if (simexchanger.ready_for_next_frame(display.refresh_rate()))
		{
			Renderables *prev, *current;
			const auto lerp = simexchanger.get_simstates(&prev, &current, display.refresh_rate());

			/*
			if (current->menu_renderables.empty() && cursor_enabled)
			{
				display.cursor(false);
				cursor_enabled = false;
			}
			else if (!current->menu_renderables.empty() && !cursor_enabled)
			{
				display.cursor(true);
				cursor_enabled = true;
			}
			*/

			renderer.render(*prev, *current, lerp, input.y, display.refresh_rate());
			display.swap();
		}
	}

	return 0;
}
