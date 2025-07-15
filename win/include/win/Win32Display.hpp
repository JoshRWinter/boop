#pragma once

#include <win/Win.hpp>

#ifdef WINPLAT_WINDOWS

#include <vector>

#include <gl/GL.h>
#include <GL/wglext.h>

#include <win/DisplayBase.hpp>

namespace win
{

class Win32Display : public DisplayBase
{
	WIN_NO_COPY_MOVE(Win32Display);

	struct MonitorProperties
	{
		std::string name;
		int width;
		int height;
		float rate;
	};

public:
	explicit Win32Display(const DisplayOptions &options);
	~Win32Display() override;

	void process() override;
	void swap() override;
	int width() override;
	int height() override;
	int screen_width() override;
	int screen_height() override;
	float refresh_rate() override;
	void cursor(bool show) override;
	void vsync(bool on) override;
	NativeWindowHandle native_handle() override;

private:
	static LRESULT CALLBACK wndproc(HWND, UINT, WPARAM, LPARAM);
	void win_init_gl(HWND);
	void win_term_gl();
	void init_monitor_properties();
	void update_refresh_rate();

	HWND window;
	HDC hdc;
	HGLRC context;
	int gl_major;
	int gl_minor;
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;
	std::vector<MonitorProperties> monprops;
	float rrate;
};

}

#endif
