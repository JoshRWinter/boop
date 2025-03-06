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
	GLCommonRenderer(win::AssetRoll &roll, const glm::mat4 &projection, const win::Dimensions<int> &screenres, const win::Area<float> &area);
	virtual ~GLCommonRenderer()	= default;

	void draw(const std::vector<const Renderable*> &renderables, const std::vector<LightRenderable> &lights);

private:
	void world_to_screen(float x, float y, int &xi, int &yi);
	static void generate_vertex_data(const win::Atlas &atlas, std::vector<float> &posdata, std::vector<unsigned short> &texcoord);

	win::Dimensions<int> screenres;
	win::Area<float> area;

	win::GLFramebuffer render;
	win::GLTexture render_att1;
	win::GLTexture render_att2;

	glm::mat4 projection;

	win::GLAtlas atlas;
	win::GLProgram program;
	GLint uniform_light;
	GLint uniform_lightcolor;
	GLint uniform_lightpower;
	GLint uniform_luminance;
	GLint uniform_transform;
	GLint uniform_color;
	GLint uniform_history_color;

	win::GLVertexArray vao;
	win::GLBuffer vbo_pos;
	win::GLBuffer vbo_texcoord;
};
