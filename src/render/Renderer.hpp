#pragma once

#include <vector>
#include <memory>

#include <win/AssetRoll.hpp>

#include "RendererBackend.hpp"

class Renderer
{
public:
	explicit Renderer(win::AssetRoll &roll);

	void render(const std::vector<Renderable> &renderables) { backend->render(renderables); }

private:
	std::unique_ptr<RendererBackend> backend;
};
