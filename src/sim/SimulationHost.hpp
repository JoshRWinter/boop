#pragma once

#include <vector>
#include <atomic>

#include <win/Win.hpp>
#include <win/ConcurrentRingBuffer.hpp>
#include <win/SimStateExchanger.hpp>

#include "../SyncObjectManager.hpp"
#include "../render/Renderable.hpp"
#include "../Input.hpp"

class SimulationHost
{
	WIN_NO_COPY_MOVE(SimulationHost);

public:
	SimulationHost(std::atomic<bool> &simquit, SyncObjectManager<Input, 3> &input, win::ConcurrentRingBuffer<char, 20> &textinput, win::SimStateExchanger<Renderables> &simexchanger);

	Renderables &get_renderables();
	void release_renderables_and_sleep(Renderables &renderables);

	Input get_input();
	void get_text_input(std::vector<char> &input);

	bool quit();
	void stop();

private:
	std::atomic<bool> &simquit;
	SyncObjectManager<Input, 3> &input;
	win::ConcurrentRingBuffer<char, 20> &textinput;
	win::SimStateExchanger<Renderables> &simexchanger;

	Input inputcache;
};
