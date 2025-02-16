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
	post.uniform_tex = get_uniform(post.program, "tex");

	histfade.program = win::GLProgram(win::load_gl_shaders(roll["shader/gl/histfade.vert"], roll["shader/gl/histfade.frag"]));

	win::gl_check_error();
}

void GLPostProcessingRenderer::draw(GLuint fb)
{
	glBindVertexArray(vao.get());

	glColorMaski(0, GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(histfade.program.get());
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
