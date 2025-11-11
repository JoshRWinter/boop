#pragma once

#include <vector>

#include <glm/glm.hpp>

#include <win/Win.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GLAtlas.hpp>
#include <win/gl/GL.hpp>
#include <win/gl/GLTextRenderer.hpp>

#include "../Renderable.hpp"

class GLMenuRenderer
{
	WIN_NO_COPY_MOVE(GLMenuRenderer);

public:
	GLMenuRenderer(win::AssetRoll &roll, win::GLTextRenderer &text_renderer, win::GLFont &menufont_teeny, win::GLFont &menufont_small, win::GLFont &menufont_big, const glm::mat4 &projection);

	void draw(const std::vector<MenuRenderable> &menu_renderables, const std::vector<TextRenderable> &text_renderables, GLuint main, GLuint fb_scratch);
	void set_res_area(const win::Dimensions<int> &res, const win::Area<float> &area);

private:
	void generate_vertex_data(const win::Atlas &atlas, std::vector<float> &posdata, std::vector<unsigned short> &texcoord);

	glm::mat4 projection;

	win::GLTextRenderer &text_renderer;
	win::GLFont &menufont_teeny, &menufont_small, &menufont_big;

	win::GLProgram program;
	int uniform_transform;
	int uniform_color;
	int uniform_horizontal;
	int uniform_blurtex;

	win::GLAtlas atlas;

	win::GLVertexArray vao;
	win::GLBuffer verts;
	win::GLBuffer texcoords;
};
