#include <chrono>
#include <thread>

#include "SimulationHost.hpp"

SimulationHost::SimulationHost(std::atomic<bool> &simquit, SyncObjectManager<Renderables, 3> &renderables, SyncObjectManager<Input, 3> &input, win::ConcurrentRingBuffer<char, 20> &textinput)
	: simquit(simquit)
	, renderables(renderables)
	, input(input)
	, textinput(textinput)
	, last_wake(std::chrono::high_resolution_clock::now())
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
	renderables.time = std::chrono::high_resolution_clock::now();
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
	// sleep for 16.666 milliseconds (60hz)

	float consumed;
	do
	{
		consumed = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - last_wake).count();

		if (consumed < 10.0f)
		{
			std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
		else
		{
			// don't sleep this close to the deadline
		}
	} while (consumed < 160.666f);

	last_wake = std::chrono::high_resolution_clock::now();
}
