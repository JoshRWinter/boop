#include <win/Display.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>
#include <win/FrameTimingCalculator.hpp>

#include "render/Renderer.hpp"
#include "sim/Simulation.hpp"

#if defined WINPLAT_WINDOWS && NDEBUG
int WinMain(HINSTANCE hinstance, HINSTANCE prev, PSTR cmd, int show)
#else
int main(int argc, char **argv)
#endif
{
	win::AssetRoll roll("boop.roll");

	// display setup
	win::DisplayOptions display_options;
#ifndef NDEBUG
	display_options.caption = "debug_window";
	display_options.fullscreen = false;
	display_options.width = 1600;
	display_options.height = 900;
	display_options.debug = false;
#else
	display_options.caption = "Darktimes";
	display_options.fullscreen = true;
	display_options.width = 1600;
	display_options.height = 900;
#endif
	display_options.gl_major = 4;
	display_options.gl_minor = 4;

	win::Display display(display_options);
	display.vsync(true);
	//display.cursor(false);

	win::load_gl_functions();

	const win::Dimensions<int> screenres(display.width(), display.height());
	const win::Area<float> area(-8.0f, 8.0f, -4.5f, 4.5f);

	const auto beginning = std::chrono::high_resolution_clock::now();

	Simulation sim(area, false, DifficultyLevel::easy, win::SimSpeedRegulator(beginning));
	Renderer renderer(roll, screenres, area);
	win::FrameTimingCalculator ftc(beginning);

	Input input;
	bool input_available = false;

	std::vector<char> text_buffer;
	bool text_available = false;

	bool quit = false;
	display.register_button_handler([&quit, &input_available, &input, &text_buffer, &text_available](win::Button button, bool press)
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

			default: break;
		}
	});

	display.register_character_handler([&text_buffer, &text_available](int c)
	{
		if (c >= ' ' && c <= '~')
		{
			text_buffer.push_back(c);
			text_available = true;
		}
	});

	display.register_mouse_handler([&input, &input_available, &area, &screenres](int x, int y)
	{
		input.x = ((x / (float)screenres.width) * (area.right - area.left)) + area.left;
		input.y = -(((y / (float)screenres.height) * (area.top - area.bottom)) + area.bottom);
		input_available = true;
	});

	Renderables *prev_renderables = NULL;
	Renderables *current_renderables = NULL;

	while ((prev_renderables = sim.get_renderables()) == NULL) ;
	while ((current_renderables = sim.get_renderables()) == NULL) ;

	bool cursor_enabled = true;

	while (!quit)
	{
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

		if (ftc.ready_for_next_frame(display.refresh_rate()))
		{
			auto new_renderables = sim.get_renderables();
			if (new_renderables != NULL)
			{
				sim.release_renderables(prev_renderables);

				prev_renderables = current_renderables;
				current_renderables = new_renderables;

#ifndef WINPLAT_LINUX
				if (current_renderables->menu_renderables.empty() && cursor_enabled)
				{
					display.cursor(false);
					cursor_enabled = false;
				}
				else if (!current_renderables->menu_renderables.empty() && !cursor_enabled)
				{
					display.cursor(true);
					cursor_enabled = true;
				}
#endif
			}

			const auto lerp_t = ftc.get_lerp_t(prev_renderables->time, current_renderables->time, display.refresh_rate(), 60.0f);
			//fprintf(stderr, "%.4f\n", lerp_t);

			renderer.render(*prev_renderables, *current_renderables, lerp_t, input.y);
			display.swap();
		}
	}

	return 0;
}
