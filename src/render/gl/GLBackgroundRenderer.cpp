#include <win/Targa.hpp>

#include "GLConstants.hpp"
#include "GLBackgroundRenderer.hpp"

using namespace win::gl;

static int get_uniform(const win::GLProgram &program, const char *name)
{
	const auto loc = glGetUniformLocation(program.get(), name);

	if (loc == -1)
		win::bug("no uniform " + std::string(name));

	return loc;
}

GLBackgroundRenderer::GLBackgroundRenderer(win::AssetRoll &roll, const win::Dimensions<int> &res, const win::Area<float> &area)
	: res(res)
	, area(area)
{
	program = win::GLProgram(load_gl_shaders(roll["shader/gl/background.vert"], roll["shader/gl/background.frag"]));
	glUseProgram(program.get());

	uniform_light = get_uniform(program, "light");
	uniform_lightcolor = get_uniform(program, "lightcolor");
	uniform_lightpower = get_uniform(program, "lightpower");
	const auto uniform_sampler = get_uniform(program, "tex");

	glUniform1i(uniform_sampler, GLConstants::BACKGROUND_TEXTURE_UNIT - GL_TEXTURE0);

	glBindVertexArray(vao.get());

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glActiveTexture(GLConstants::BACKGROUND_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, texture.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	win::Targa tga(roll["texture/background.tga"]);
	if (tga.bpp() != 24)
		win::bug("texture too fat");

	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tga.width(), tga.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, tga.data());
}

void GLBackgroundRenderer::set_res_area(const win::Dimensions<int> &res, const win::Area<float> &area)
{
	this->res = res;
	this->area = area;
}

void GLBackgroundRenderer::draw(const std::vector<LightRenderable> &lights)
{
	glUseProgram(program.get());
	glBindVertexArray(vao.get());

	if (lights.empty())
	{
		glUniform2f(uniform_light, 0.0f, 0.0f);
		glUniform3f(uniform_lightcolor, 0.0f, 0.0f, 0.0f);
		glUniform1f(uniform_lightpower, 0.0f);
	}
	else
	{
		int x, y;
		world_to_screen(lights[0].x, lights[0].y, x, y);
		glUniform2f(uniform_light, x, y);
		glUniform3f(uniform_lightcolor, lights[0].color.red, lights[0].color.green, lights[0].color.blue);
		glUniform1f(uniform_lightpower, (res.width / 10.0f) * lights[0].power);
	}

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void GLBackgroundRenderer::world_to_screen(float x, float y, int &xi, int &yi)
{
	xi = ((x / (area.right - area.left)) * res.width) + (res.width / 2.0f);
	yi = ((y / (area.top - area.bottom)) * res.height) + (res.height / 2.0f);
}
