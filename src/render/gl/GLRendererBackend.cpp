#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLRendererBackend.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(win::AssetRoll &roll, const win::Area<float> &area)
	: buckets(5)
	, common_renderer(roll, glm::ortho(area.left, area.right, area.bottom, area.top))
{
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.01f, 0.01f, 0.012f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void GLRendererBackend::render(const std::vector<Renderable> &renderables, const std::vector<MenuRenderable> &menu_renderables)
{
	glClear(GL_COLOR_BUFFER_BIT);

	for (const auto &r : renderables)
		buckets.at(r.layer).renderables.push_back(&r);

	for (const auto &r : menu_renderables)
		buckets.at(0).menu_renderables.push_back(&r);

	for (const auto &bucket : buckets)
	{
		common_renderer.draw(bucket.renderables);
	}

	win::gl_check_error();
}
