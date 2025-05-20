#include <cmath>

#include <win/FrameTimingCalculator.hpp>

namespace win
{

float FrameTimingCalculator::get_lerp_t(std::chrono::time_point<std::chrono::high_resolution_clock> prev, std::chrono::time_point<std::chrono::high_resolution_clock> current, float refresh_rate, bool &ready)
{
	const auto prev_time = std::chrono::duration<float>(prev - start).count();
	const auto current_time = std::chrono::duration<float>(current - start).count();
	const auto now = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count();

	const auto ft = 1.0f / refresh_rate;
	const float next_vblank = (now - std::fmod(now, ft)) + ft;

	const auto between = next_vblank - (1.0f / std::max(60.0f, refresh_rate));
	const float t = (between - prev_time) / (current_time - prev_time);

	ready = next_vblank != last_vblank;
	last_vblank = next_vblank;
	return t;
}

}
