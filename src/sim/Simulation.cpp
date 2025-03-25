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

void Simulation::sim(win::Area<float> area, bool runbot)
{
	SimulationHost host(quit, som_renderables, som_input, textinput);
	Game game(area, runbot);
	game.play(host);
}
