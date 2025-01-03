#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/FileReadStream.hpp>

#include "GLRendererBackend.hpp"
#include "GL.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(win::AssetRoll &roll, const win::Dimensions<int> &screenres, const win::Area<float> &area)
	: buckets(5)
	, text_renderer(screenres, area, FONT_TEXTURE_UNIT, true, 0, true)
	, menufont_small(screenres, area, 0.5f, win::Stream(new win::FileReadStream("/usr/share/fonts/noto/NotoSansMono-Regular.ttf")))
	, menufont_big(screenres, area, 1.0f, win::Stream(new win::FileReadStream("/usr/share/fonts/noto/NotoSansMono-Regular.ttf")))
	, common_renderer(roll, glm::ortho(area.left, area.right, area.bottom, area.top))
	, menu_renderer(roll, text_renderer, menufont_small, menufont_big, glm::ortho(area.left, area.right, area.bottom, area.top))
{
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.01f, 0.01f, 0.012f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void GLRendererBackend::render(const std::vector<Renderable> &renderables, const std::vector<MenuRenderable> &menu_renderables, const std::vector<TextRenderable> &text_renderables)
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (const auto &r : renderables)
		buckets.at(r.layer).renderables.push_back(&r);

	for (const auto &r : menu_renderables)
		buckets.at(0).menu_renderables.push_back(&r);

	for (const auto &r : text_renderables)
		buckets.at(0).text_renderables.push_back(&r);

	for (auto &bucket : buckets)
	{
		common_renderer.draw(bucket.renderables);
		menu_renderer.draw(bucket.menu_renderables, bucket.text_renderables);

		bucket.clear();
	}

	win::gl_check_error();
}
