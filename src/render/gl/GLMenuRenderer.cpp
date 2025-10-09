#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLMenuRenderer.hpp"
#include "GLConstants.hpp"

using namespace win::gl;

static int get_uniform(win::GLProgram &program, const char *name)
{
	const auto loc = glGetUniformLocation(program.get(), name);
	if (loc == -1)
		win::bug("no uniform " + std::string(name));

	return loc;
}

GLMenuRenderer::GLMenuRenderer(win::AssetRoll &roll, win::GLTextRenderer &text_renderer, win::GLFont &menufont_teeny, win::GLFont &menufont_small, win::GLFont &menufont_big, const glm::mat4 &projection)
	: projection(projection)
	, text_renderer(text_renderer)
	, menufont_teeny(menufont_teeny)
	, menufont_small(menufont_small)
	, menufont_big(menufont_big)
	, program(win::load_gl_shaders(roll["shader/gl/menu.vert"], roll["shader/gl/menu.frag"]))
	, atlas(roll["texture/menuatlas"], win::GLAtlas::Mode::linear, GLConstants::MENU_ATLAS_TEXTURE_UNIT)
{
	glUseProgram(program.get());
	uniform_transform = get_uniform(program, "transform");
	uniform_color = get_uniform(program, "color");
	glUniform1i(get_uniform(program, "tex"), GLConstants::MENU_ATLAS_TEXTURE_UNIT - GL_TEXTURE0);

	glBindVertexArray(vao.get());

	std::vector<float> vertdata;
	std::vector<unsigned short> tcdata;
	generate_vertex_data(atlas, vertdata, tcdata);

	glBindBuffer(GL_ARRAY_BUFFER, verts.get());
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertdata.size(), vertdata.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, texcoords.get());
	glEnableVertexAttribArray(1);
	glVertexAttribIPointer(1, 2, GL_UNSIGNED_SHORT, 0, NULL);
	glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned short) * tcdata.size(), tcdata.data(), GL_STATIC_DRAW);

	win::gl_check_error();
}

void GLMenuRenderer::draw(const std::vector<MenuRenderable> &menu_renderables, const std::vector<TextRenderable> &text_renderables)
{
	glUseProgram(program.get());
	glBindVertexArray(vao.get());

	static const auto ident = glm::identity<glm::mat4>();

	for (const auto &r : menu_renderables)
	{
		const auto translate = glm::translate(ident, glm::vec3(r.x + (r.w / 2.0f), r.y + (r.h / 2.0f), 0.0f));
		const auto scale = glm::scale(ident, glm::vec3(r.w, r.h, 1.0f));
		const auto transform = projection * translate * scale;

		glUniform4f(uniform_color, r.color.red, r.color.green, r.color.blue, r.color.alpha);
		glUniformMatrix4fv(uniform_transform, 1, GL_FALSE, glm::value_ptr(transform));

		glDrawArrays(GL_TRIANGLES, (int)r.texture * 6, 6);
	}

	for (const auto &r : text_renderables)
	{
		const win::GLFont *f;
		if (r.texttype == TextRenderable::Type::teeny)
			f = &menufont_teeny;
		else if (r.texttype == TextRenderable::Type::smol)
			f = &menufont_small;
		else
			f = &menufont_big;

		text_renderer.draw(*f, r.text, r.x, r.y, r.color, r.centered);
	}

	text_renderer.flush();

	win::gl_check_error();
}

void GLMenuRenderer::generate_vertex_data(const win::Atlas &atlas, std::vector<float> &posdata, std::vector<unsigned short> &texcoord)
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
