#pragma once

#include <thread>
#include <atomic>
#include <vector>

#include <win/Win.hpp>
#include <win/Utility.hpp>

#include "../SyncObjectManager.hpp"
#include "../render/Renderable.hpp"
#include "../Input.hpp"

class Simulation
{
	WIN_NO_COPY_MOVE(Simulation);

public:
	Simulation(const win::Area<float> &area, bool runbot);
	~Simulation();

	RenderableCollection *get_renderables();
	void release_renderables(RenderableCollection *renderables);
	void set_input(Input input);

private:
	void sim(win::Area<float> area, bool runbot);
	void sleep();

	SyncObjectManager<RenderableCollection, 3> som_renderables;
	SyncObjectManager<Input, 3> som_input;

	bool bot;
	std::atomic<bool> quit;
	std::thread simthread;
};
