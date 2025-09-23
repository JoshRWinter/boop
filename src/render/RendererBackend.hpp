#pragma once

#include <vector>

#include <win/Win.hpp>
#include <win/AssetRoll.hpp>

#include "Renderable.hpp"

class RendererBackend
{
	WIN_NO_COPY_MOVE(RendererBackend);

public:
	RendererBackend() = default;
	virtual ~RendererBackend() = default;

	virtual void render(const std::vector<Renderable> &renderables, const std::vector<LightRenderable> &light_renderables, const std::vector<MenuRenderable> &menu_renderables, const std::vector<TextRenderable> &text_renderables, float fps) = 0;
	virtual void set_resolution(const win::Dimensions<int> &dims, win::AssetRoll &roll) = 0;
};
