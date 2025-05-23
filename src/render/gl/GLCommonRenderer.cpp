#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <win/gl/GL.hpp>

#include "GLCommonRenderer.hpp"
#include "GLConstants.hpp"

using namespace win::gl;

static GLint get_uniform(win::GLProgram &program, const char *name)
{
	const auto loc = glGetUniformLocation(program.get(), name);
	if (loc == -1)
		win::bug("No uniform " + std::string(name));

	return loc;
}

GLCommonRenderer::GLCommonRenderer(win::AssetRoll &roll, const glm::mat4 &projection, const win::Dimensions<int> &screenres, const win::Area<float> &area)
	: screenres(screenres)
	, area(area)
	, projection(projection)
	, atlas(roll["texture/atlas"], win::GLAtlas::Mode::linear, GLConstants::ATLAS_TEXTURE_UNIT)
	, program(win::load_gl_shaders(roll["shader/gl/common.vert"], roll["shader/gl/common.frag"]))
{
	glUseProgram(program.get());
	uniform_light = get_uniform(program, "light");
	uniform_lightcolor = get_uniform(program, "lightcolor");
	uniform_lightpower = get_uniform(program, "lightpower");
	uniform_luminance = get_uniform(program, "luminance");
	uniform_transform = get_uniform(program, "transform");
	uniform_color = get_uniform(program, "color");
	uniform_history_color = get_uniform(program, "history_color");
	glUniform1i(get_uniform(program, "tex"), GLConstants::ATLAS_TEXTURE_UNIT - GL_TEXTURE0);

	glBindVertexArray(vao.get());

	std::vector<float> posdata;
	std::vector<unsigned short> texcoorddata;
	generate_vertex_data(atlas, posdata, texcoorddata);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_pos.get());
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * posdata.size(), posdata.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord.get());
	glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, 0, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned short) * texcoorddata.size(), texcoorddata.data(), GL_STATIC_DRAW);

	win::gl_check_error();
}

void GLCommonRenderer::draw(const std::vector<Renderable> &renderables, const std::vector<LightRenderable> &lights)
{
	glUseProgram(program.get());
	glBindVertexArray(vao.get());

	static const auto ident = glm::identity<glm::mat4>();

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
		glUniform1f(uniform_lightpower, (screenres.width / 10.0f) * lights[0].power);
	}

	for (const auto &renderable : renderables)
	{
		const auto translate = glm::translate(ident, glm::vec3(renderable.x + (renderable.w / 2.0f), renderable.y + (renderable.h / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(renderable.w, renderable.h, 1.0f));
		const auto rotate = glm::rotate(ident, renderable.rot, glm::vec3(0.0f, 0.0f, 1.0f));

		const auto transform = projection * translate * rotate * scale;
		glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(transform));
		glUniform4f(uniform_color, renderable.color.red, renderable.color.green, renderable.color.blue, renderable.color.alpha);
		glUniform4f(uniform_history_color, renderable.history_color.red, renderable.history_color.green, renderable.history_color.blue, renderable.history_color.alpha);
		glUniform1f(uniform_luminance, renderable.emissiveness);

		glDrawArrays(GL_TRIANGLES, (int)renderable.texture * 6, 6);
	}
}

void GLCommonRenderer::world_to_screen(float x, float y, int &xi, int &yi)
{
	xi = ((x / (area.right - area.left)) * screenres.width) + (screenres.width / 2.0f);
	yi = ((y / (area.top - area.bottom)) * screenres.height) + (screenres.height / 2.0f);
}

void GLCommonRenderer::generate_vertex_data(const win::Atlas &atlas, std::vector<float> &posdata, std::vector<unsigned short> &texcoord)
{
	const auto toshort = [](const float f) { return (unsigned short)std::roundf(f * 65535.0f); };

	for (int i = 0; i < atlas.count(); ++i)
	{
		const float half = 0.5f;
		const auto &item = atlas.item(i);

		posdata.push_back(-half);
		posdata.push_back(half);
		texcoord.push_back(toshort(item.x1));
		texcoord.push_back(toshort(item.y2));

		posdata.push_back(-half);
		posdata.push_back(-half);
		texcoord.push_back(toshort(item.x1));
		texcoord.push_back(toshort(item.y1));

		posdata.push_back(half);
		posdata.push_back(-half);
		texcoord.push_back(toshort(item.x2));
		texcoord.push_back(toshort(item.y1));

		posdata.push_back(-half);
		posdata.push_back(half);
		texcoord.push_back(toshort(item.x1));
		texcoord.push_back(toshort(item.y2));

		posdata.push_back(half);
		posdata.push_back(-half);
		texcoord.push_back(toshort(item.x2));
		texcoord.push_back(toshort(item.y1));

		posdata.push_back(half);
		posdata.push_back(half);
		texcoord.push_back(toshort(item.x2));
		texcoord.push_back(toshort(item.y2));
	}
}
