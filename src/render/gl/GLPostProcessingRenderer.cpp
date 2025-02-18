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

GLPostProcessingRenderer::GLPostProcessingRenderer(win::AssetRoll &roll, const win::Dimensions<int> &screenres)
{
	post.program = win::GLProgram(win::load_gl_shaders(roll["shader/gl/post.vert"], roll["shader/gl/post.frag"]));
	glUseProgram(post.program.get());
	post.uniform_tex = get_uniform(post.program, "tex");

	blur.program = win::GLProgram(win::load_gl_shaders(roll["shader/gl/blur.vert"], roll["shader/gl/blur.frag"]));
	glUseProgram(blur.program.get());
	blur.uniform_texture = get_uniform(blur.program, "tex");
	blur.uniform_horizontal = get_uniform(blur.program, "horizontal");
	glBindFramebuffer(GL_FRAMEBUFFER, blur.fbo.get());
	const GLenum buffers[] { GL_NONE, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, buffers);
	glActiveTexture(GLConstants::BLUR_COLOR_ATTACHMENT_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, blur.tex.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16, screenres.width, screenres.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, blur.tex.get(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	histfade.program = win::GLProgram(win::load_gl_shaders(roll["shader/gl/histfade.vert"], roll["shader/gl/histfade.frag"]));

	win::gl_check_error();
}

void GLPostProcessingRenderer::draw(GLuint fb)
{
	glBindVertexArray(vao.get());

	glColorMaski(0, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	glUseProgram(histfade.program.get());
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindFramebuffer(GL_FRAMEBUFFER, blur.fbo.get());
	glUseProgram(blur.program.get());
	glUniform1i(blur.uniform_texture, GLConstants::HISTORY_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);
	glUniform1i(blur.uniform_horizontal, 1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glUniform1i(blur.uniform_texture, GLConstants::BLUR_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);
	glUniform1i(blur.uniform_horizontal, 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(post.program.get());
	glUniform1i(post.uniform_tex, GLConstants::MAIN_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glUniform1i(post.uniform_tex, GLConstants::HISTORY_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	win::gl_check_error();
}
