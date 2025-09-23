#pragma once

#include <vector>

#include "../RendererBackend.hpp"
#include "GLCommonRenderer.hpp"
#include "GLMenuRenderer.hpp"
#include "GLPostProcessingRenderer.hpp"

class GLRendererBackend : public RendererBackend
{
public:
	GLRendererBackend(win::AssetRoll &roll, const win::Dimensions<int> &screenres, const win::Area<float> &area);
	~GLRendererBackend() override = default;

	void render(const std::vector<Renderable> &renderables, const std::vector<LightRenderable> &light_renderables, const std::vector<MenuRenderable> &menu_renderables, const std::vector<TextRenderable> &text_renderables, float fps) override;
	void set_resolution(const win::Dimensions<int> &dims, win::AssetRoll &roll) override;

private:
	void drawfps();

	win::GLTextRenderer text_renderer;
	win::GLFont menufont_tiny, menufont_small, menufont_big;

	const win::Area<float> area;

	GLCommonRenderer common_renderer;
	GLMenuRenderer menu_renderer;
	GLPostProcessingRenderer post_renderer;

	win::GLFramebuffer fb;
	win::GLTexture fb_main, fb_history;
};
