#pragma once

#include <vector>
#include <atomic>

#include <win/Win.hpp>
#include <win/ConcurrentRingBuffer.hpp>

#include "../SyncObjectManager.hpp"
#include "../render/Renderable.hpp"
#include "../Input.hpp"

class SimulationHost
{
	WIN_NO_COPY_MOVE(SimulationHost);

public:
	SimulationHost(std::atomic<bool> &simquit, SyncObjectManager<Renderables, 4> &renderables, SyncObjectManager<Input, 3> &input, win::ConcurrentRingBuffer<char, 20> &textinput);

	Renderables &get_renderables();
	void release_renderables(Renderables &renderables);

	Input get_input();
	void get_text_input(std::vector<char> &input);

	bool quit();
	void sleep();

private:
	std::atomic<bool> &simquit;
	SyncObjectManager<Renderables, 4> &renderables;
	SyncObjectManager<Input, 3> &input;
	win::ConcurrentRingBuffer<char, 20> &textinput;

	std::chrono::time_point<std::chrono::high_resolution_clock> last_wake;

	Input inputcache;
};
