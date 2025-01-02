#include <chrono>

#include "Simulation.hpp"
#include "Game.hpp"

Simulation::Simulation(const win::Area<float> &area, bool runbot)
	: bot(runbot)
	, quit(false)
{
	simthread = std::thread([area, runbot, this](){ sim(area, runbot); });
}

Simulation::~Simulation()
{
	quit = true;
	simthread.join();
}

RenderableCollection *Simulation::get_renderables()
{
	return som_renderables.reader_acquire();
}

void Simulation::release_renderables(RenderableCollection *renderables)
{
	som_renderables.reader_release(renderables);
}

void Simulation::set_input(Input input)
{
	Input *obj = NULL;
	do { obj = som_input.writer_acquire(); } while (obj == NULL);

	*obj = input;

	som_input.writer_release(obj);
}

void Simulation::sim(win::Area<float> area, bool runbot)
{
	std::unique_ptr<Simulation> botsim;

	if (!runbot)
	{
		botsim.reset(new Simulation(area, true));
	}

	Game game(area, !runbot, runbot);

	Input input;

	while (!quit.load())
	{
		RenderableCollection *renderables = NULL;
		while (renderables == NULL)
			renderables = som_renderables.writer_acquire();

		Input *latest_input;
		if ((latest_input = som_input.reader_acquire()) != NULL)
		{
			input = *latest_input;
			som_input.reader_release(latest_input);
		}

		renderables->renderables.clear();
		game.tick(renderables->renderables, input);
		renderables->time = std::chrono::high_resolution_clock::now();

		som_renderables.writer_release(renderables);

		sleep();
	}
}

void Simulation::sleep()
{
	const auto start = std::chrono::high_resolution_clock::now();

	while (std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - start).count() < 16.666f)
	{
#ifndef WINPLAT_WINDOWS
		std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(16.666));
#endif
	}
}
