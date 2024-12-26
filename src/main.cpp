#include <win/Display.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>

#include "render/Renderer.hpp"

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

	bool quit = false;
	display.register_button_handler([&quit](win::Button button, bool press) { if (button == win::Button::esc && press) quit = true; });

	win::load_gl_functions();

	Renderer renderer(roll, win::Area<float>(-8.0f, 8.0f, -4.5, 4.5f));

	while (!quit)
	{
		display.process();

		renderer.render({ Renderable(Texture::ball, 0.0f, 0.0f, 1.0f, 1.0f, win::Color<float>(1.0f, 0.0f, 0.0f, 1.0f) )});
		display.swap();
	}

	return 0;
}
