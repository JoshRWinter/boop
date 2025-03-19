#pragma once

#include <win/Utility.hpp>

enum class Color
{
	red,
	green,
	blue,
	yellow,
	cyan,
	magenta,
	last = magenta
};

inline win::Color<float> get_red() { return win::Color<float>(0.8f, 0.2f, 0.2f); }
inline win::Color<float> get_green() { return win::Color<float>(0.2f, 0.8f, 0.2f); }
inline win::Color<float> get_blue() { return win::Color<float>(0.2f, 0.2f, 0.8f); }
inline win::Color<float> get_yellow() { return win::Color<float>(0.8f, 0.8f, 0.2f); }
inline win::Color<float> get_cyan() { return win::Color<float>(0.2f, 0.8f, 0.8f); }
inline win::Color<float> get_magenta() { return win::Color<float>(0.8f, 0.2f, 0.8f); }

inline win::Color<float> get_color(Color c)
{
	switch (c)
	{
		case Color::red: return get_red();
		case Color::green: return get_green();
		case Color::blue: return get_blue();
		case Color::yellow: return get_yellow();
		case Color::cyan: return get_cyan();
		case Color::magenta: return get_magenta();
		default: return get_red();
	}
}
