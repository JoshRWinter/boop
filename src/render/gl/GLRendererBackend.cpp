#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/FileReadStream.hpp>

#include "GLConstants.hpp"
#include "GLRendererBackend.hpp"
#include "GLConstants.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(win::AssetRoll &roll, const win::Dimensions<int> &screenres, const win::Area<float> &area)
	: buckets(5)
	, text_renderer(screenres, area, GLConstants::FONT_TEXTURE_UNIT, true, GLConstants::TEXT_RENDERER_UNIFORM_BLOCK_BINDING, true)
	, menufont_small(screenres, area, 0.5f, win::Stream(new win::FileReadStream("/usr/share/fonts/noto/NotoSansMono-Regular.ttf")))
	, menufont_big(screenres, area, 1.0f, win::Stream(new win::FileReadStream("/usr/share/fonts/noto/NotoSansMono-Regular.ttf")))
	, common_renderer(roll, glm::ortho(area.left, area.right, area.bottom, area.top))
	, menu_renderer(roll, text_renderer, menufont_small, menufont_big, glm::ortho(area.left, area.right, area.bottom, area.top))
	, post_renderer(roll)
{
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClearColor(0.01f, 0.01f, 0.012f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, fb.get());

	glActiveTexture(GLConstants::MAIN_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, fb_main.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, screenres.width, screenres.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_main.get(), 0);

	glActiveTexture(GLConstants::HISTORY_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, fb_history.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, screenres.width, screenres.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fb_history.get(), 0);

	const GLenum buf[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buf);

	const GLint clearcolor[] = { 0, 0, 0, 0 };
	glClearBufferiv(GL_COLOR, 1, clearcolor);

	win::gl_check_error();
}

void GLRendererBackend::render(const std::vector<Renderable> &renderables, const std::vector<MenuRenderable> &menu_renderables, const std::vector<TextRenderable> &text_renderables)
{
	const GLint clearcolor[] = { 0, 0, 0, 0 };
	glClearBufferiv(GL_COLOR, 0, clearcolor);
	//glClearBufferiv(GL_COLOR, 1, clearcolor);

	for (const auto &r : renderables)
		buckets.at(r.layer).renderables.push_back(&r);

	for (const auto &r : menu_renderables)
		buckets.at(0).menu_renderables.push_back(&r);

	for (const auto &r : text_renderables)
		buckets.at(0).text_renderables.push_back(&r);

	for (auto &bucket : buckets)
	{
		glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		common_renderer.draw(bucket.renderables);

		glColorMaski(1, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		menu_renderer.draw(bucket.menu_renderables, bucket.text_renderables);

		bucket.clear();
	}

	glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	post_renderer.draw(fb.get());

	win::gl_check_error();
}
