#pragma once

#include <glm/glm.hpp>

#include <win/Win.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GLAtlas.hpp>

#include "../Renderable.hpp"

class GLCommonRenderer
{
	WIN_NO_COPY_MOVE(GLCommonRenderer);

public:
	GLCommonRenderer(win::AssetRoll &roll, const glm::mat4 &projection);
	virtual ~GLCommonRenderer()	= default;

	void draw(const std::vector<const Renderable*> &renderables);

private:
	static void generate_vertex_data(const win::Atlas &atlas, std::vector<float> &posdata, std::vector<unsigned short> &texcoord);

	win::GLFramebuffer render;
	win::GLTexture render_att1;
	win::GLTexture render_att2;

	glm::mat4 projection;

	win::GLAtlas atlas;
	win::GLProgram program;
	int uniform_transform;
	int uniform_color;

	win::GLVertexArray vao;
	win::GLBuffer vbo_pos;
	win::GLBuffer vbo_texcoord;
};
