#include <chrono>

#include "Game.hpp"
#include "Simulation.hpp"

Simulation::Simulation(win::AssetRoll *roll,
					   const win::Area<float> &area,
					   bool runbot,
					   DifficultyLevel bot_difficulty,
					   win::SimStateExchanger<Renderables> &simexchanger)
	: bot(runbot)
	, quit(false)
{
	simthread = std::thread(
		[roll, area, runbot, bot_difficulty, &simexchanger, this]
		{
			sim(roll, area, runbot, bot_difficulty, simexchanger);
		});
}

Simulation::~Simulation()
{
	quit = true;
	simthread.join();
}

void Simulation::set_input(const Input &input)
{
	Input *obj = NULL;
	do
	{
		obj = exch_input.writer_acquire();
	} while (obj == NULL);

	*obj = input;

	exch_input.writer_release(obj);
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

void Simulation::sim(win::AssetRoll *roll,
					 win::Area<float> area,
					 bool runbot,
					 DifficultyLevel bot_difficulty,
					 win::SimStateExchanger<Renderables> &simexchanger)
{
	Game game(roll, area, runbot, bot_difficulty);

	Input input;

	while (!quit)
	{
		auto &renderables = simexchanger.prepare_simstate();
		renderables.clear();

		{
			auto *i = exch_input.reader_acquire();
			if (i != NULL)
			{
				input = *i;
				exch_input.reader_release(i);
			}
		}

		std::vector<char> text_input;

		{
			char buf[std::remove_reference_t<decltype(textinput)>::length];
			const auto read = textinput.read(buf, sizeof(buf));

			for (int i = 0; i < read; ++i)
				text_input.push_back(buf[i]);
		}

		const bool exit = !game.play(renderables, input, text_input);

		simexchanger.release_simstate_and_sleep(renderables);

		if (exit)
			quit.store(true);
	}
}
