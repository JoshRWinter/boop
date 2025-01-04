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

Renderables *Simulation::get_renderables()
{
	return som_renderables.reader_acquire();
}

void Simulation::release_renderables(Renderables *renderables)
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
	Game game(area, runbot);

	Input input;

	while (!quit.load())
	{
		Renderables *renderables = NULL;
		while (renderables == NULL)
			renderables = som_renderables.writer_acquire();
		renderables->clear();

		Input *latest_input;
		if ((latest_input = som_input.reader_acquire()) != NULL)
		{
			input = *latest_input;
			som_input.reader_release(latest_input);
		}

		game.play(*renderables, input);
		renderables->time = std::chrono::high_resolution_clock::now();

		som_renderables.writer_release(renderables);

		sleep();
	}
}

void Simulation::sleep()
{
	const auto start = std::chrono::high_resolution_clock::now();

	std::this_thread::sleep_for(std::chrono::milliseconds(12));

	while (std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - start).count() < 16.666f);
}
