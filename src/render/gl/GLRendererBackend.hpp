#pragma once

#include <vector>

#include "../RendererBackend.hpp"
#include "GLCommonRenderer.hpp"
#include "GLMenuRenderer.hpp"

class GLRendererBackend : public RendererBackend
{
public:
	GLRendererBackend(win::AssetRoll &roll, const win::Dimensions<int> &screenres, const win::Area<float> &area);
	~GLRendererBackend() override = default;

	void render(const std::vector<Renderable> &renderables, const std::vector<MenuRenderable> &menu_renderables, const std::vector<TextRenderable> &text_renderables) override;

private:
	struct LayerBucket
	{
		std::vector<const Renderable*> renderables;
		std::vector<const MenuRenderable*> menu_renderables;
		std::vector<const TextRenderable*> text_renderables;
		void clear()
		{
			renderables.clear();
			menu_renderables.clear();
			text_renderables.clear();
		}
	};

	std::vector<LayerBucket> buckets;

	win::GLTextRenderer text_renderer;
	win::GLFont menufont_small, menufont_big;

	GLCommonRenderer common_renderer;
	GLMenuRenderer menu_renderer;
};
