#pragma once

#include <functional>

struct Button
{
	static constexpr float width = 3.5f;
	static constexpr float height = 0.79166f;

	Button(float x, float y, const char *text, const char *desc, std::function<void()> &&callback)
		: x(x)
		, y(y)
		, text(text)
		, desc(desc)
		, callback(std::move(callback))
		, click(false)
	{
	}

	float x, y;
	const char *text;
	const char *desc;
	std::function<void()> callback;
	bool click;
};
