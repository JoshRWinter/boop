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

GLPostProcessingRenderer::GLPostProcessingRenderer(win::AssetRoll &roll)
{
	post.program = win::GLProgram(win::load_gl_shaders(roll["shader/gl/post.vert"], roll["shader/gl/post.frag"]));
	glUseProgram(post.program.get());
	post.uniform_main_texture = get_uniform(post.program, "main_texture");
	post.uniform_history_texture = get_uniform(post.program, "history_texture");
	post.uniform_blur_horizontal = get_uniform(post.program, "horizontal");
	glUniform1i(post.uniform_main_texture, GLConstants::MAIN_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);

	histfade.program = win::GLProgram(win::load_gl_shaders(roll["shader/gl/histfade.vert"], roll["shader/gl/histfade.frag"]));
	histfade.uniform_fade = glGetUniformLocation(histfade.program.get(), "fade");

	win::gl_check_error();
}

void GLPostProcessingRenderer::draw(GLuint mainfb, GLuint scratchfb, float fps)
{
	glBindVertexArray(vao.get());

	glColorMaski(0, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glUseProgram(histfade.program.get());
	glUniform1f(histfade.uniform_fade, 0.4f / (fps / 60.0f));
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glColorMaski(0, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glUseProgram(post.program.get());

	// horizontal blur
	glBindFramebuffer(GL_FRAMEBUFFER, scratchfb);
	glUniform1i(post.uniform_history_texture, GLConstants::HISTORY_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);
	glUniform1i(post.uniform_blur_horizontal, 1);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	// vertical blur and other post processing
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniform1i(post.uniform_history_texture, GLConstants::SCRATCH_COLOR_ATTACHMENT_TEXTURE_UNIT - GL_TEXTURE0);
	glUniform1i(post.uniform_blur_horizontal, 0);

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, mainfb);

	win::gl_check_error();
}
