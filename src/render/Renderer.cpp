#include "Renderer.hpp"
#include "gl/GLRendererBackend.hpp"

Renderer::Renderer(win::AssetRoll &roll, const win::Dimensions<int> &screenres, const win::Area<float> &area)
	: backend(new GLRendererBackend(roll, screenres, area))
{}

void Renderer::render(const Renderables &renderables, std::chrono::high_resolution_clock::time_point last)
{
	const auto interval = std::chrono::duration<float, std::milli>(renderables.time - last).count();
	const auto diff = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - renderables.time).count();

	const auto t = diff / interval;

	// lerp the lerpables
	lerped.clear();
	for (const auto &r : renderables.lerped_renderables)
		lerped.emplace_back(r.layer, r.texture, lerp(r.old_x, r.x, t), lerp(r.old_y, r.y, t), lerp(r.old_width, r.w, t), lerp(r.old_height, r.h, t), r.color);

	lerped_lights.clear();
	for (const auto &r : renderables.light_renderables)
		lerped_lights.emplace_back(lerp(r.old_x, r.x, t), lerp(r.old_y, r.y, t), r.color, r.power);

	backend->render(lerped, lerped_lights, renderables.menu_renderables, renderables.text_renderables);
}

float Renderer::lerp(float a, float b, float t)
{
	return ((b - a) * t) + a;
}
