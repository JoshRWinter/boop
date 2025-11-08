#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLConstants.hpp"
#include "GLRendererBackend.hpp"
#include "GLConstants.hpp"

using namespace win::gl;

GLRendererBackend::GLRendererBackend(win::AssetRoll &roll, const win::Dimensions<int> &screenres, const win::Area<float> &area)
	: text_renderer(screenres, area, GLConstants::FONT_TEXTURE_UNIT, true, GLConstants::TEXT_RENDERER_SHADER_STORAGE_BLOCK_BINDING, true)
	, menufont_tiny(text_renderer.create_font(0.125f, roll["font/Comicy.ttf"]))
	, menufont_smaller(text_renderer.create_font(0.25f, roll["font/Comicy.ttf"]))
	, menufont_small(text_renderer.create_font(0.5f, roll["font/Comicy.ttf"]))
	, menufont_big(text_renderer.create_font(1.0f, roll["font/Comicy.ttf"]))
	, background_renderer(roll, screenres, area)
	, common_renderer(roll, glm::ortho(area.left, area.right, area.bottom, area.top), screenres, area)
	, menu_renderer(roll, text_renderer, menufont_smaller, menufont_small, menufont_big, glm::ortho(area.left, area.right, area.bottom, area.top))
	, post_renderer(roll)
{
	/*
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback([](GLenum source, GLenum type, unsigned int id, GLenum severity, GLsizei length, const char *message, const void *param)
	{
		std::unique_ptr<char[]> buf(new char[length + 1]);
		memcpy(buf.get(), message, length);
		buf[length] = 0;
		fprintf(stderr, "%s\n\n", buf.get());
	}, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
	*/

	glClearColor(0.01f, 0.01f, 0.012f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, scratch.get());
	glActiveTexture(GLConstants::SCRATCH_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, fb_scratch.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, screenres.width, screenres.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_scratch.get(), 0);

	{
		const GLenum buf[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buf);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fb.get());

	glActiveTexture(GLConstants::MAIN_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, fb_main.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, screenres.width, screenres.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb_main.get(), 0);

	glActiveTexture(GLConstants::HISTORY_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, fb_history.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, screenres.width, screenres.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fb_history.get(), 0);

	{
		const GLenum buf[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, buf);
	}

	const GLfloat clearcolor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	glClearBufferfv(GL_COLOR, 1, clearcolor);

	win::gl_check_error();
}

void GLRendererBackend::render(const std::vector<Renderable> &renderables, const std::vector<LightRenderable> &light_renderables, const std::vector<MenuRenderable> &menu_renderables, const std::vector<TextRenderable> &text_renderables, float fps)
{
	background_renderer.draw(light_renderables);

	common_renderer.draw(renderables, light_renderables);

	glColorMaski(1, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	menu_renderer.draw(menu_renderables, text_renderables, fb.get(), scratch.get());

#ifndef NDEBUG
	drawfps();
#endif

	glColorMaski(1, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	post_renderer.draw(fb.get(), scratch.get(), fps);

	win::gl_check_error();
}

void GLRendererBackend::set_resolution(const win::Dimensions<int> &dims, const win::Area<float> &area, win::AssetRoll &roll)
{
	glViewport(0, 0, dims.width, dims.height);

	text_renderer.resize(dims, area);

	menufont_tiny = text_renderer.create_font(0.125f, roll["font/Comicy.ttf"]);
	menufont_smaller = text_renderer.create_font(0.25f, roll["font/Comicy.ttf"]);
	menufont_small = text_renderer.create_font(0.5f, roll["font/Comicy.ttf"]);
	menufont_big = text_renderer.create_font(1.0f, roll["font/Comicy.ttf"]);

	glActiveTexture(GLConstants::MAIN_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, fb_main.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, dims.width, dims.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glActiveTexture(GLConstants::HISTORY_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, fb_history.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, dims.width, dims.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glActiveTexture(GLConstants::SCRATCH_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, fb_scratch.get());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, dims.width, dims.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	background_renderer.set_res_area(dims, area);
}

void GLRendererBackend::drawfps()
{
	static auto last_time = std::chrono::high_resolution_clock::now();
	static int frame = 0;
	static char fpsstring[10] = "";
	++frame;

	const auto now = std::chrono::high_resolution_clock::now();
	if (std::chrono::duration<float, std::milli>(now - last_time).count() >= 1000)
	{
		snprintf(fpsstring, sizeof(fpsstring), "FPS %d", frame);
		last_time = now;
		frame = 0;
	}

	text_renderer.draw(menufont_tiny, fpsstring, -7.925f, 4.35f, win::Color<float>(0.3f, 0.3f, 0.3f));
	text_renderer.flush();
}
