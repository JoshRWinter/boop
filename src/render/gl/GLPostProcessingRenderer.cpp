#include "GLConstants.hpp"
#include "GLPostProcessingRenderer.hpp"

using namespace win::gl;

static GLint get_uniform(const win::GLProgram &program, const char *name)
{
	const auto loc = glGetUniformLocation(program.get(), name);
	if (loc == -1)
		win::bug("No uniform " + std::string(name));

	return loc;
}

GLPostProcessingRenderer::GLPostProcessingRenderer(win::AssetRoll &roll, const win::Dimensions<int> &screenres, const win::Area<float> &gamearea)
	: screenres(screenres)
	, gamearea(gamearea)
{
	post.program = win::GLProgram(win::load_gl_shaders(roll["shader/gl/post.vert"], roll["shader/gl/post.frag"]));
	glUseProgram(post.program.get());
	post.uniform_main_texture = get_uniform(post.program, "main_texture");
	post.uniform_history_texture = get_uniform(post.program, "history_texture");
	post.uniform_blur_horizontal = get_uniform(post.program, "horizontal");
	post.uniform_light = get_uniform(post.program, "light");
	post.uniform_lightcolor = get_uniform(post.program, "lightcolor");
	post.uniform_lightpower = get_uniform(post.program, "lightpower");
	post.uniform_ambientlight = get_uniform(post.program, "ambientlight");
	glUniform1i(post.uniform_main_texture, GLConstants::MAIN_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);
	glBindFramebuffer(GL_FRAMEBUFFER, post.fbo.get());
	glActiveTexture(GLConstants::BLUR_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, post.tex.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, screenres.width, screenres.height, 0, GL_RGB, GL_UNSIGNED_SHORT, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, post.tex.get(), 0);
	const GLenum buffers[] { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, buffers);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	histfade.program = win::GLProgram(win::load_gl_shaders(roll["shader/gl/histfade.vert"], roll["shader/gl/histfade.frag"]));

	win::gl_check_error();
}

void GLPostProcessingRenderer::draw(GLuint fb, const std::vector<LightRenderable> &lights)
{
	glBindVertexArray(vao.get());

	glColorMaski(0, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glUseProgram(histfade.program.get());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glUseProgram(post.program.get());

	// horizontal blur
	glBindFramebuffer(GL_FRAMEBUFFER, post.fbo.get());
	glUniform1i(post.uniform_history_texture, GLConstants::HISTORY_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);
	glUniform1i(post.uniform_blur_horizontal, 1);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// vertical blur and other post processing
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniform1i(post.uniform_history_texture, GLConstants::BLUR_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);
	glUniform1i(post.uniform_blur_horizontal, 0);

	if (lights.empty())
	{
		glUniform2f(post.uniform_light, 0.0f, 0.0f);
		glUniform3f(post.uniform_lightcolor, 0.0f, 0.0f, 0.0f);
		glUniform1f(post.uniform_lightpower, 0.0f);
		glUniform3f(post.uniform_ambientlight, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		int x, y;
		world_to_screen(lights[0].x, lights[0].y, x, y);
		glUniform2f(post.uniform_light, x, y);
		glUniform3f(post.uniform_lightcolor, lights[0].color.red, lights[0].color.green, lights[0].color.blue);
		glUniform1f(post.uniform_lightpower, (screenres.width / 1000.0f) * lights[0].power);
		glUniform3f(post.uniform_ambientlight, 0.8f, 0.8f, 0.8f);
	}

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	win::gl_check_error();
}

void GLPostProcessingRenderer::world_to_screen(float x, float y, int &xi, int &yi)
{
	xi = ((x / (gamearea.right - gamearea.left)) * screenres.width) + (screenres.width / 2.0f);
	yi = ((y / (gamearea.top - gamearea.bottom)) * screenres.height) + (screenres.height / 2.0f);
}
