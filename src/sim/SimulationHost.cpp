#include <chrono>
#include <thread>

#include "SimulationHost.hpp"

SimulationHost::SimulationHost(std::atomic<bool> &simquit, SyncObjectManager<Input, 3> &input, win::ConcurrentRingBuffer<char, 20> &textinput, win::SimStateExchanger<Renderables> &simexchanger)
	: simquit(simquit)
	, input(input)
	, textinput(textinput)
	, simexchanger(simexchanger)
{ }

Renderables &SimulationHost::get_renderables()
{
	auto &r = simexchanger.prepare_simstate();

	r.clear();
	return r;
}

void SimulationHost::release_renderables_and_sleep(Renderables &renderables)
{
	simexchanger.release_simstate_and_sleep(renderables);
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

void SimulationHost::stop()
{
	simquit.store(true);
}
