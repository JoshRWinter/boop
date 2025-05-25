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

	bool ready_for_next_frame(float refresh_frequency);
	float get_lerp_t(std::chrono::time_point<std::chrono::high_resolution_clock> prev, std::chrono::time_point<std::chrono::high_resolution_clock> current, float refresh_frequency, float sim_frequency);

private:

	std::chrono::time_point<std::chrono::high_resolution_clock> beginning = std::chrono::high_resolution_clock::now();
	long long last_vblank_nanos = 0;
};

}
