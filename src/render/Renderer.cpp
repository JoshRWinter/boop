#include "Renderer.hpp"
#include "gl/GLRendererBackend.hpp"

Renderer::Renderer(win::AssetRoll &roll, const win::Area<float> &area)
	: backend(new GLRendererBackend(roll, area))
{}

void Renderer::render(const std::vector<Renderable> &renderables, std::chrono::high_resolution_clock::time_point last, std::chrono::high_resolution_clock::time_point current)
{
	const auto interval = std::chrono::duration<float, std::milli>(current - last).count();
	const auto diff = std::chrono::duration<float, std::milli>(std::chrono::high_resolution_clock::now() - current).count();

	const auto t = diff / interval;

	interpolated.clear();
	for (const auto &item : renderables)
	{
		const float x = lerp(item.oldx, item.x, t);
		const float y = lerp(item.oldy, item.y, t);

		interpolated.emplace_back(item.texture, x, y, -1, -1, item.w, item.h, item.color);
	}

	backend->render(interpolated);
}

float Renderer::lerp(float a, float b, float t)
{
	return ((b - a) * t) + a;
}
