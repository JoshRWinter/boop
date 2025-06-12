#pragma once

#include <chrono>

#include <win/Win.hpp>

namespace win
{

class SimSpeedRegulator
{
public:
	explicit SimSpeedRegulator(std::chrono::time_point<std::chrono::high_resolution_clock> beginning);

	std::chrono::time_point<std::chrono::high_resolution_clock> sleep(int frequency);

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> beginning;

};

}
