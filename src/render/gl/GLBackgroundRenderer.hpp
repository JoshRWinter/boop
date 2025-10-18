#pragma once

#include <vector>

#include <win/Win.hpp>
#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>
#include <win/Utility.hpp>

#include "../Renderable.hpp"

class GLBackgroundRenderer
{
	WIN_NO_COPY_MOVE(GLBackgroundRenderer);

public:
	GLBackgroundRenderer(win::AssetRoll &roll, const win::Dimensions<int> &res, const win::Area<float> &area);

	void set_res_area(const win::Dimensions<int> &res, const win::Area<float> &area);
	void draw(const std::vector<LightRenderable> &lights, GLuint fbmain, GLuint fbscratch);

private:
	void world_to_screen(float x, float y, int &xi, int &yi);

	win::Dimensions<int> res;
	win::Area<float> area;

	win::GLProgram program;
	int uniform_sampler;
	int uniform_horizontal;
	int uniform_light;
	int uniform_lightcolor;
	int uniform_lightpower;

	win::GLVertexArray vao;

	win::GLTexture texture;
};
