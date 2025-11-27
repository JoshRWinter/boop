#pragma once

#include <atomic>
#include <thread>
#include <vector>

#include <win/AssetRoll.hpp>
#include <win/ConcurrentRingBuffer.hpp>
#include <win/SimStateExchanger.hpp>
#include <win/Utility.hpp>
#include <win/Win.hpp>

#include "../Input.hpp"
#include "../render/Renderable.hpp"
#include "../SyncObjectManager.hpp"
#include "Difficulty.hpp"

class Simulation
{
	WIN_NO_COPY_MOVE(Simulation);

public:
	Simulation(win::AssetRoll *roll,
			   const win::Area<float> &area,
			   bool runbot,
			   DifficultyLevel bot_difficulty,
			   win::SimStateExchanger<Renderables> &simexchanger);
	~Simulation();

	void set_input(const Input &input);
	void set_text_input(const std::vector<char> &text);
	bool should_quit();

private:
	void sim(win::AssetRoll *roll, win::Area<float> area, bool runbot, DifficultyLevel bot_difficulty, win::SimStateExchanger<Renderables> &simexchanger);

	SyncObjectManager<Input, 4> exch_input;
	win::ConcurrentRingBuffer<char, 20> textinput;
	Renderables *cached = NULL;

	bool bot;
	std::atomic<bool> quit;
	std::thread simthread;
};
