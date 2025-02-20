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
		lerped.emplace_back(r.layer, r.texture, lerp(r.old_x, r.x, t), lerp(r.old_y, r.y, t), lerp(r.old_width, r.w, t), lerp(r.old_height, r.h, t), lerp(r.old_rot, r.rot, t), r.color);

	backend->render(lerped, renderables.menu_renderables, renderables.text_renderables);
}

float Renderer::lerp(float a, float b, float t)
{
	return ((b - a) * t) + a;
}
