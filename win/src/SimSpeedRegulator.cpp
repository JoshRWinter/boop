#include <thread>

#include <win/SimSpeedRegulator.hpp>

win::SimSpeedRegulator::SimSpeedRegulator(std::chrono::time_point<std::chrono::high_resolution_clock> beginning)
	: beginning(beginning)
{ }

std::chrono::time_point<std::chrono::high_resolution_clock> win::SimSpeedRegulator::sleep(int frequency)
{
	auto now = std::chrono::high_resolution_clock::now();

	const long long tick_length_nanos = (1.0 / frequency) * 1'000'000'000;
	const auto nanos = std::chrono::duration<long long, std::nano>(now - beginning).count();

	const auto beginning_of_sim_tick = now - std::chrono::nanoseconds(nanos % tick_length_nanos);
	const auto end_of_sim_tick = beginning_of_sim_tick + std::chrono::nanoseconds(tick_length_nanos);
	const auto end_of_next_sim_tick = end_of_sim_tick + std::chrono::nanoseconds(tick_length_nanos);

	const auto shrink = 10000000.0f;
	fprintf(stderr, "SIM RELEASE:\nbeginning_of_sim_tick: %.4f\nend_of_sim_tick: %.4f\nend_of_next_sim_tick: %.4f\nnow: %.4f\n\n\n\n", std::chrono::duration<long long, std::nano>(beginning_of_sim_tick - beginning).count() / shrink, std::chrono::duration<long long, std::nano>(end_of_sim_tick - beginning).count() / shrink, std::chrono::duration<long long, std::nano>(end_of_next_sim_tick - beginning).count() / shrink, nanos / shrink);

	// sleep until end_of_sim_tick
	while (true)
	{
		const auto due_nanos = std::chrono::duration<long long, std::nano>(end_of_sim_tick - now).count();

		if (due_nanos < 1)
		{
			// Overdue, return;
			return end_of_next_sim_tick;
		}

		std::this_thread::sleep_for(std::chrono::nanoseconds(due_nanos));

		now = std::chrono::high_resolution_clock::now();
	}
}

