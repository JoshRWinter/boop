#pragma once

#include <vector>

#include <win/Win.hpp>

#include "Renderable.hpp"

class RendererBackend
{
	WIN_NO_COPY_MOVE(RendererBackend);

public:
	RendererBackend() = default;
	virtual ~RendererBackend() = default;

	virtual void render(const std::vector<Renderable> &renderables, const std::vector<LightRenderable> &light_renderables, const std::vector<MenuRenderable> &menu_renderables, const std::vector<TextRenderable> &text_renderables) = 0;
};
