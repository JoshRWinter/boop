#pragma once

#include <chrono>
#include <thread>
#include <cstdint>

#include <win/Win.hpp>
#include <win/ObjectExchanger.hpp>

namespace win
{

namespace impl
{

template <typename SimState> struct SimStateContainer : SimState
{
	std::int64_t time;
};

}

template <typename SimState> class SimStateExchanger
{
	WIN_NO_COPY_MOVE(SimStateExchanger);

public:
	explicit SimStateExchanger(float sim_frequency)
		: sim_frequency(sim_frequency)
	{}

	// Call this from simulation thread
	SimState &prepare_simstate()
	{
		impl::SimStateContainer<SimState> *ss;

		do
		{
			ss = exchanger.writer_acquire();
		} while (ss == NULL);

		return *ss;
	}

	// Call this from simulation thread
	void release_simstate_and_sleep(SimState &simstate)
	{
		std::int64_t end;
		get_interval_bounds_nanos(sim_frequency, NULL, &end);

		static auto last_end = end;

		if (end - last_end > (((1.0 / sim_frequency) * 1'000'000'000)) * 1.1)
			fprintf(stderr, "MISSED A SIM\n");

		last_end = end;

		auto container = (impl::SimStateContainer<SimState>*)&simstate;
		container->time = end;

		exchanger.writer_release(container);

		std::int64_t remaining_nanos;
		while (true)
		{
			remaining_nanos = end - std::chrono::duration<std::int64_t, std::nano>(std::chrono::high_resolution_clock::now() - beginning).count();

			if (remaining_nanos <= 0)
				break;

			//std::this_thread::sleep_for(std::chrono::nanoseconds(remaining_nanos));
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
	}

	// Call this from consuming thread
	float get_simstates(SimState **prev, SimState **current, float refreshrate)
	{
		std::int64_t current_vblank;
		get_interval_bounds_nanos(refreshrate, &current_vblank, NULL);

		if (current_vblank - last_vblank > ((1.0 / refreshrate) * 1'000'000'000) * 1.1)
			fprintf(stderr, "MISSED A VBLANK\n");

		last_vblank = current_vblank;

		if (current_simstate == NULL) // first time
		{
			if (prev_simstate != NULL)
				win::bug("SimStateExchanger: somehow prev and current are null and not null");

			while (prev_simstate == NULL)
				prev_simstate = exchanger.reader_acquire();

			while (current_simstate == NULL)
				current_simstate = exchanger.reader_acquire();
		}
		else
		{
			auto newest = exchanger.reader_acquire();
			if (newest != NULL)
			{
				exchanger.reader_release(prev_simstate);

				prev_simstate = current_simstate;
				current_simstate = newest;
			}
		}

		*prev = prev_simstate;
		*current = current_simstate;

		std::int64_t simstart, simend;
		get_interval_bounds_nanos(sim_frequency, &simstart, &simend);

		const auto simtime = (double)((1.0 / sim_frequency) * 1'000'000'000);
		return (current_vblank - prev_simstate->time) / simtime;
	}

	bool ready_for_next_frame(float refreshrate)
	{
		std::int64_t current_vblank;
		get_interval_bounds_nanos(refreshrate, &current_vblank, NULL);

		return current_vblank != last_vblank;
	}

private:
	void get_interval_bounds_nanos(float frequency, std::int64_t *start_nanos, std::int64_t *end_nanos)
	{
		const auto now = std::chrono::high_resolution_clock::now();
		const auto now_nanos = std::chrono::duration<std::int64_t, std::nano>(now - beginning).count();

		const std::int64_t period = (1.0f / frequency) * 1'000'000'000;
		const auto start = now_nanos - (now_nanos % period);
		const auto end = start + period;

		if (start_nanos != NULL)
			*start_nanos = start;

		if (end_nanos)
			*end_nanos = end;
	}

	const float sim_frequency;
	const std::chrono::time_point<std::chrono::high_resolution_clock> beginning = std::chrono::high_resolution_clock::now();

	win::ObjectExchanger<impl::SimStateContainer<SimState>, 10> exchanger;

	impl::SimStateContainer<SimState> *prev_simstate = NULL;
	impl::SimStateContainer<SimState> *current_simstate = NULL;

	std::int64_t last_vblank = 0;
};

}
