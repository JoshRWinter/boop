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

	const auto between = next_vblank - get_simulation_delta(prev, current);
	const float t = (between - prev_time) / (current_time - prev_time);

	fprintf(stderr, "%.4f\n", get_simulation_delta(prev, current));

	ready = next_vblank != last_vblank;
	last_vblank = next_vblank;
	return t;
}

float FrameTimingCalculator::get_simulation_delta(std::chrono::time_point<std::chrono::high_resolution_clock> prev, std::chrono::time_point<std::chrono::high_resolution_clock> current)
{
	const float ms = std::chrono::duration<float, std::milli>(current - prev).count();

	if (ms < 25.0f)
		return 1.0f / 60.0f;
	else if (ms < 50.0f)
		return 1.0f / 30.0f;
	else
		return 1.0f / 15.0f;
}

}
