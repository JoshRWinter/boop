#pragma once

#include <vector>

#include "../RendererBackend.hpp"
#include "GLCommonRenderer.hpp"

class GLRendererBackend : public RendererBackend
{
public:
	explicit GLRendererBackend(win::AssetRoll &roll, const win::Area<float> &area);
	~GLRendererBackend() override = default;

	void render(const std::vector<Renderable> &renderables, const std::vector<MenuRenderable> &menu_renderables) override;

private:
	struct LayerBucket
	{
		std::vector<const Renderable*> renderables;
		std::vector<const MenuRenderable*> menu_renderables;
		void clear()
		{
			renderables.clear();
			menu_renderables.clear();
		}
	};

	std::vector<LayerBucket> buckets;
	GLCommonRenderer common_renderer;
};
