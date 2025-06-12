#include <chrono>
#include <thread>

#include "SimulationHost.hpp"

SimulationHost::SimulationHost(std::atomic<bool> &simquit, SyncObjectManager<Renderables, 4> &renderables, SyncObjectManager<Input, 3> &input, win::ConcurrentRingBuffer<char, 20> &textinput, win::SimSpeedRegulator simspeed)
	: simquit(simquit)
	, renderables(renderables)
	, input(input)
	, textinput(textinput)
	, simspeed(simspeed)
{ }

Renderables &SimulationHost::get_renderables()
{
	Renderables *r;

	do
	{
		r = renderables.writer_acquire();
	} while (r == NULL);

	r->clear();
	return *r;
}

void SimulationHost::release_renderables(Renderables &renderables)
{
	renderables.time = end_of_current_sim_tick;
	this->renderables.writer_release(&renderables);
}

Input SimulationHost::get_input()
{
	const auto i = input.reader_acquire();
	if (i)
	{
		inputcache = *i;
		input.reader_release(i);
	}

	return inputcache;
}

void SimulationHost::get_text_input(std::vector<char> &input)
{
	char buf[std::remove_reference_t<decltype(textinput)>::length];
	const auto read = textinput.read(buf, sizeof(buf));

	for (int i = 0; i < read; ++i)
		input.push_back(buf[i]);
}

bool SimulationHost::quit()
{
	return simquit.load();
}

void SimulationHost::sleep()
{
	// end of current sim tick AFTER the sleep, that is
	end_of_current_sim_tick = simspeed.sleep(60);
}
