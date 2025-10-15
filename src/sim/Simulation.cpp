#include <chrono>

#include "Simulation.hpp"
#include "Game.hpp"

Simulation::Simulation(win::AssetRoll *roll, const win::Area<float> &area, bool runbot, DifficultyLevel bot_difficulty, win::SimStateExchanger<Renderables> &simexchanger)
	: bot(runbot)
	, quit(false)
{
	simthread = std::thread([roll, area, runbot, bot_difficulty, &simexchanger, this]{ sim(roll, area, runbot, bot_difficulty, simexchanger); });
}

Simulation::~Simulation()
{
	quit = true;
	simthread.join();
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

bool Simulation::should_quit()
{
	return quit.load();
}

void Simulation::sim(win::AssetRoll *roll, win::Area<float> area, bool runbot, DifficultyLevel bot_difficulty, win::SimStateExchanger<Renderables> &simexchanger)
{
	SimulationHost host(quit, som_input, textinput, simexchanger);
	Game game(roll, area, runbot, bot_difficulty);
	game.play(host);
}
