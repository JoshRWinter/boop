#pragma once

#include <thread>
#include <atomic>
#include <vector>

#include <win/Win.hpp>
#include <win/Utility.hpp>
#include <win/ConcurrentRingBuffer.hpp>
#include <win/SimStateExchanger.hpp>

#include "../SyncObjectManager.hpp"
#include "../render/Renderable.hpp"
#include "../Input.hpp"
#include "Difficulty.hpp"

class Simulation
{
	WIN_NO_COPY_MOVE(Simulation);

public:
	Simulation(const win::Area<float> &area, bool runbot, DifficultyLevel bot_difficulty, win::SimStateExchanger<Renderables> &simexchanger);
	~Simulation();

	void set_input(const Input &input);
	void set_text_input(const std::vector<char> &text);

private:
	void sim(win::Area<float> area, bool runbot, DifficultyLevel bot_difficulty, win::SimStateExchanger<Renderables> &simexchanger);

	SyncObjectManager<Renderables, 4> som_renderables;
	SyncObjectManager<Input, 3> som_input;
	win::ConcurrentRingBuffer<char, 20> textinput;
	Renderables *cached = NULL;

	bool bot;
	std::atomic<bool> quit;
	std::thread simthread;
};
