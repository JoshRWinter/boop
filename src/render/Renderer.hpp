#pragma once

#include <vector>
#include <memory>

#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

#include "RendererBackend.hpp"

class Renderer
{
public:
	Renderer(win::AssetRoll &roll, const win::Area<float> &area);

	void render(const std::vector<Renderable> &renderables) { backend->render(renderables); }

private:
	std::unique_ptr<RendererBackend> backend;
};
