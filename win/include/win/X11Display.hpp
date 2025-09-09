#pragma once

#include <win/Win.hpp>

#ifdef WINPLAT_LINUX

#include <X11/Xlib.h>
#include <GL/glx.h>

#include <win/DisplayBase.hpp>

namespace win
{

class X11Display : public DisplayBase
{
	WIN_NO_COPY_MOVE(X11Display);

public:
	explicit X11Display(const DisplayOptions &options);
	~X11Display() override;

	void process() override;
	void swap() override;
	int width() override;
	int height() override;
	int screen_width() override;
	int screen_height() override;
	float refresh_rate() override;
	void cursor(bool show) override;
	void vsync(bool on) override;
	void set_fullscreen(bool fulscreen) override;
	NativeWindowHandle native_handle() override;

private:
	void update_refresh_rate();

	Window window;
	GLXContext context;
	PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT;

	struct
	{
		int lastx = 0, lasty = 0;
		unsigned lastwidth = 0, lastheight = 0;
		float rrate = 60.0f;
	} rrate_cache;
};

}

#endif
