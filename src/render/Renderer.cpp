#include "Renderer.hpp"
#include "gl/GLRendererBackend.hpp"

Renderer::Renderer(win::AssetRoll &roll, const win::Dimensions<int> &screenres, const win::Area<float> &area)
	: backend(new GLRendererBackend(roll, screenres, area))
{}

void Renderer::render(const Renderables &prev, const Renderables &current, float mousey)
{
	const auto now = std::chrono::high_resolution_clock::now();
	const auto time_since_current = std::chrono::duration<float, std::milli>(now - current.time).count();
	const auto interval = std::chrono::duration<float, std::milli>(current.time - prev.time).count();
	const auto t = time_since_current / interval;

	lerped_renderables.clear();
	lerped_lights.clear();

	for (const auto &r : current.renderables)
	{
		const Renderable *old = NULL;

		// find the old one
		for (const auto &r2 : prev.renderables)
		{
			if (r2.id == r.id)
			{
				old = &r2;
				break;
			}
		}

		if (old != NULL)
		{
			const float y = r.id == current.player_controlled_id ? mousey - (r.h / 2.0f) : r.y;
			const float old_y = r.id == current.player_controlled_id ? y : old->y;

			lerped_renderables.emplace_back(
				r.id,
				r.texture,
				lerp(old->x, r.x, t),
				lerp(old_y, y, t),
				lerp(old->w, r.w, t),
				lerp(old->h, r.h, t),
				lerp(old->rot, r.rot, t),
				lerp(old->emissiveness, r.emissiveness, t),
				lerp(old->color, r.color, t),
				lerp(old->history_color, r.history_color, t));
		}
		else
		{
			lerped_renderables.push_back(r);
		}
	}

	for (const auto &r : current.light_renderables)
	{
		const LightRenderable *old = NULL;

		// find the old one
		for (const auto &r2 : prev.light_renderables)
		{
			if (r2.id == r.id)
			{
				old = &r2;
				break;
			}
		}

		if (old != NULL)
		{
			lerped_lights.emplace_back(
				r.id,
				lerp(old->x, r.x, t),
				lerp(old->y, r.y, t),
				lerp(old->color, r.color, t),
				lerp(old->power, r.power, t));
		}
		else
		{
			lerped_lights.push_back(r);
		}
	}

	backend->render(lerped_renderables, lerped_lights, current.menu_renderables, current.text_renderables);
}

float Renderer::lerp(float a, float b, float t)
{
	return ((b - a) * t) + a;
}

win::Color<float> Renderer::lerp(const win::Color<float> &a, const win::Color<float> &b, float t)
{
	return win::Color(lerp(a.red, b.red, t), lerp(a.green, b.green, t), lerp(a.blue, b.blue, t), lerp(a.alpha, b.alpha, t));
}
