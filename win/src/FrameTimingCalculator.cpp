#include <cmath>

#include <win/FrameTimingCalculator.hpp>

namespace win
{

FrameTimingCalculator::FrameTimingCalculator(std::chrono::time_point<std::chrono::high_resolution_clock> beginning)
	: beginning(beginning)
{ }

bool FrameTimingCalculator::ready_for_next_frame(float refresh_frequency)
{
	const auto now = std::chrono::high_resolution_clock::now();
	const auto now_nanos = std::chrono::duration<long long, std::nano>(now - beginning).count();

	const long long frametime_nanos = (1.0 / refresh_frequency) * 1'000'000'000;

	const auto prev_vblank_nanos = now_nanos - (now_nanos % frametime_nanos);
	const auto next_vblank_nanos = prev_vblank_nanos + frametime_nanos;

	const bool ready = next_vblank_nanos != last_vblank_nanos;
	last_vblank_nanos = next_vblank_nanos;

	return ready;
}

float FrameTimingCalculator::get_lerp_t(std::chrono::time_point<std::chrono::high_resolution_clock> prev, std::chrono::time_point<std::chrono::high_resolution_clock> current, float refresh_frequency, float sim_frequency)
{
	const auto now = std::chrono::high_resolution_clock::now();

	const auto prev_time_nanos = std::chrono::duration<long long, std::nano>(prev - beginning).count();
	const auto current_time_nanos = std::chrono::duration<long long, std::nano>(current - beginning).count();
	const auto now_nanos = std::chrono::duration<long long, std::nano>(now - beginning).count();

	const long long sim_frequency_nanos = (1.0 / sim_frequency) * 1'000'000'000;
	const long long frametime_nanos = (1.0 / refresh_frequency) * 1'000'000'000;

	const auto prev_vblank_nanos = now_nanos - (now_nanos % frametime_nanos);
	const auto next_vblank_nanos = prev_vblank_nanos + frametime_nanos;

	const auto between_nanos = next_vblank_nanos - sim_frequency_nanos;
	const float t = (between_nanos - prev_time_nanos) / (double)(current_time_nanos - prev_time_nanos);

	//const auto shrink = 10000000.0f;
	//fprintf(stderr, "prevsim: %.4f\ncursim: %.4f\nnow: %.4f\nprevvblank: %.4f\nnextvblank: %.4f\nbetween: %.4f\nt: %.4f\n\n\n\n\n", prev_time_nanos / shrink, current_time_nanos / shrink, now_nanos / shrink, prev_vblank_nanos / shrink, next_vblank_nanos / shrink, between_nanos / shrink, t);

	return t;
}

}
