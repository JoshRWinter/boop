#pragma once

#include <memory>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

#include "RendererBackend.hpp"

class Renderer
{
public:
	Renderer(win::AssetRoll &roll, const win::Area<float> &area);

	void render(const std::vector<Renderable> &renderables, std::chrono::high_resolution_clock::time_point last, std::chrono::high_resolution_clock::time_point current);

private:
	static float lerp(float a, float b, float t);

	std::unique_ptr<RendererBackend> backend;
	std::vector<Renderable> interpolated;
};
