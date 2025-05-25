#pragma once

#include <chrono>

#include <win/Win.hpp>

namespace win
{

class SimSpeedRegulator
{
	WIN_NO_COPY_MOVE(SimSpeedRegulator);

public:
	SimSpeedRegulator() = default;

	std::chrono::time_point<std::chrono::high_resolution_clock> sleep(int frequency);

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> beginning = std::chrono::high_resolution_clock::now();

};

}
