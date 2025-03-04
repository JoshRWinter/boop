#pragma once

#include <memory>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

#include "RendererBackend.hpp"

class Renderer
{
public:
	Renderer(win::AssetRoll &roll, const win::Dimensions<int> &screen_area, const win::Area<float> &area);

	void render(const Renderables &renderables, std::chrono::high_resolution_clock::time_point last);

private:
	static float lerp(float a, float b, float t);

	std::vector<Renderable> lerped;
	std::vector<LightRenderable> lerped_lights;
	std::unique_ptr<RendererBackend> backend;
};
