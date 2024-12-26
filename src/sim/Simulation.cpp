#include <chrono>

#include "Simulation.hpp"
#include "Game.hpp"

Simulation::Simulation(const win::Area<float> &area)
	: quit(false)
{
	simthread = std::thread([&](){ sim(area); });
}

Simulation::~Simulation()
{
	quit = true;
	simthread.join();
}

std::vector<Renderable> *Simulation::get_renderables()
{
	return som.reader_acquire();
}

void Simulation::release_renderables(std::vector<Renderable> *renderables)
{
	som.reader_release(renderables);
}

void Simulation::sim(win::Area<float> area)
{
	Game game(area);

	while (!quit.load())
	{
		std::vector<Renderable> *renderables = NULL;
		while (renderables == NULL)
			renderables = som.writer_acquire();

		renderables->clear();
		game.tick(*renderables);

		som.writer_release(renderables);

		sleep();
	}
}

void Simulation::sleep()
{
	std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(16.666));
}
