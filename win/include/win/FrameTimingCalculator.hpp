#pragma once

#include <chrono>

#include <win/Win.hpp>

namespace win
{

class FrameTimingCalculator
{
	WIN_NO_COPY_MOVE(FrameTimingCalculator);

public:
	FrameTimingCalculator() = default;
	float get_lerp_t(std::chrono::time_point<std::chrono::high_resolution_clock> prev, std::chrono::time_point<std::chrono::high_resolution_clock> current, float refresh_rate, bool &ready);

private:
	static float get_simulation_delta(std::chrono::time_point<std::chrono::high_resolution_clock> prev, std::chrono::time_point<std::chrono::high_resolution_clock> current);

	std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();
	float last_vblank = 0.0f;
};

}
