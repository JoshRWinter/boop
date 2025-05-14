#pragma once

#include <memory>
#include <chrono>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>
#include <win/FrameTimingCalculator.hpp>

#include "RendererBackend.hpp"

class Renderer
{
public:
	Renderer(win::AssetRoll &roll, const win::Dimensions<int> &screen_area, const win::Area<float> &area);

	bool render(const Renderables &prev, const Renderables &current, float mousey);

private:
	static float lerp(float a, float b, float t);
	static win::Color<float> lerp(const win::Color<float> &a, const win::Color<float> &b, float t);

	std::unique_ptr<RendererBackend> backend;
	win::FrameTimingCalculator ftc;

	std::vector<Renderable> lerped_renderables;
	std::vector<LightRenderable> lerped_lights;
};
