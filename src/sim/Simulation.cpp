#include <chrono>

#include "Simulation.hpp"
#include "Game.hpp"

Simulation::Simulation(const win::Area<float> &area, bool runbot, DifficultyLevel bot_difficulty, win::SimSpeedRegulator simspeed)
	: bot(runbot)
	, quit(false)
{
	simthread = std::thread([area, runbot, bot_difficulty, simspeed, this]{ sim(area, runbot, bot_difficulty, simspeed); });
}

Simulation::~Simulation()
{
	quit = true;
	simthread.join();
}

Renderables *Simulation::get_renderables()
{
	const auto now = std::chrono::high_resolution_clock::now();
	auto t = som_renderables.reader_acquire();

	if (t != NULL)
	{
		if (t->time <= now)
		{
			if (cached != NULL)
			{
				som_renderables.reader_release(cached);
				cached = NULL;
			}

			return t;
		}
		else
		{
			if (cached != NULL)
			{
				som_renderables.reader_release(cached);
			}

			cached = t;

			return NULL;
		}
	}
	else
	{
		if (cached != NULL && cached->time <= now)
		{
				auto tmp = cached;
				cached = NULL;
				return tmp;
		}
		else
			return NULL;
	}
}

void Simulation::release_renderables(Renderables *renderables)
{
	som_renderables.reader_release(renderables);
}

void Simulation::set_input(const Input &input)
{
	Input *obj = NULL;
	do { obj = som_input.writer_acquire(); } while (obj == NULL);

	*obj = input;

	som_input.writer_release(obj);
}

void Simulation::set_text_input(const std::vector<char> &text)
{
	int written = 0;
    while (written != text.size())
    	written += textinput.write(text.data() + written, text.size() - written);
}

void Simulation::sim(win::Area<float> area, bool runbot, DifficultyLevel bot_difficulty, win::SimSpeedRegulator simspeed)
{
	SimulationHost host(quit, som_renderables, som_input, textinput, simspeed);
	Game game(area, runbot, bot_difficulty);
	game.play(host);
}
