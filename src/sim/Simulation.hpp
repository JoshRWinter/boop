#pragma once

#include <thread>
#include <atomic>
#include <vector>

#include <win/Win.hpp>
#include <win/Utility.hpp>

#include "../SyncObjectManager.hpp"
#include "../render/Renderable.hpp"

class Simulation
{
	WIN_NO_COPY_MOVE(Simulation);

public:
	Simulation(const win::Area<float> &area);
	~Simulation();

	std::vector<Renderable> *get_renderables();
	void release_renderables(std::vector<Renderable> *renderables);

private:
	void sim(win::Area<float> area);
	void sleep();

	SyncObjectManager<std::vector<Renderable>, 3> som;

	std::atomic<bool> quit;
	std::thread simthread;
};
