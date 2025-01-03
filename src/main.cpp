#include <win/Display.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "render/Renderer.hpp"
#include "sim/Simulation.hpp"

int main()
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
	display_options.width = 1;
	display_options.height = 1;
#endif
	display_options.gl_major = 3;
	display_options.gl_minor = 3;

	win::Display display(display_options);
	display.vsync(true);
	//display.cursor(false);

	win::load_gl_functions();

	const win::Dimensions<int> screenres(display.width(), display.height());
	const win::Area<float> area(-8.0f, 8.0f, -4.5f, 4.5f);

	Simulation sim(area, false);
	Renderer renderer(roll, screenres, area);

	bool quit = false;
	Input input;
	bool input_available = false;

	display.register_button_handler([&quit, &input, &input_available](win::Button button, bool press)
	{
		switch (button)
		{
			case win::Button::esc:
				quit = true;
				input_available = true;
				break;
			case win::Button::mouse_left:
				input.click = press;
				input_available = true;
				break;

			default: break;
		}
	});

	display.register_mouse_handler([&input, &input_available, &area, &screenres](int x, int y)
	{
		input.x = ((x / (float)screenres.width) * (area.right - area.left)) + area.left;
		input.y = -(((y / (float)screenres.height) * (area.top - area.bottom)) + area.bottom);
		input_available = true;
	});

	Renderables *renderables = NULL;
	while ((renderables = sim.get_renderables()) == NULL) ;
	auto lasttime = renderables->time;

	while (!quit)
	{
		display.process();

		if (input_available)
		{
			input_available = false;
			sim.set_input(input);
		}

		auto new_renderables = sim.get_renderables();
		if (new_renderables != NULL)
		{
			lasttime = renderables->time;
			sim.release_renderables(renderables);
			renderables = new_renderables;
		}

		renderer.render(*renderables, lasttime);

		display.swap();
	}

	return 0;
}
