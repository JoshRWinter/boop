#pragma once

#include <win/AssetRoll.hpp>

#include "../RendererBackend.hpp"

class GLRendererBackend : public RendererBackend
{
public:
	explicit GLRendererBackend(win::AssetRoll &roll);
	~GLRendererBackend() override = default;

	void render(const std::vector<Renderable> &renderables) override;

private:
	std::vector<Renderable> renderables;
};
