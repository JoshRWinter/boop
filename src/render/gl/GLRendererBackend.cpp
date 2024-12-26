#include <cmath>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLRendererBackend.hpp"

using namespace win::gl;

static GLint get_uniform(win::GLProgram &program, const char *name)
{
	const auto loc = glGetUniformLocation(program.get(), name);
	if (loc == -1)
		win::bug("No uniform " + std::string(name));

	return loc;
}

GLRendererBackend::GLRendererBackend(win::AssetRoll &roll, const win::Area<float> &area)
	: atlas(roll["texture/atlas"], win::GLAtlas::Mode::linear)
	, program(win::load_gl_shaders(roll["shader/gl/common.vert"], roll["shader/gl/common.frag"]))
{
	glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(program.get());
	projection = glm::ortho(area.left, area.right, area.bottom, area.top);
	uniform_transform = get_uniform(program, "transform");

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

void GLRendererBackend::render(const std::vector<Renderable> &renderables)
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program.get());
	glBindVertexArray(vao.get());
	glBindTexture(GL_TEXTURE_2D, atlas.texture());

	static const auto ident = glm::identity<glm::mat4>();

	for (const auto &renderable : renderables)
	{
		const auto translate = glm::translate(ident, glm::vec3(renderable.x + (renderable.w / 2.0f), renderable.y + (renderable.h / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(renderable.w, renderable.h, 1.0f));

		const auto transform = projection * translate * scale;
		glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(transform));

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void GLRendererBackend::generate_vertex_data(const win::Atlas &atlas, std::vector<float> &posdata, std::vector<unsigned short> &texcoord)
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
