#pragma once

#include <vector>
#include <memory>
#include <chrono>

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
	void draw(const std::vector<LightRenderable> &lights);

private:
	void world_to_screen(float x, float y, int &xi, int &yi);
	static std::unique_ptr<unsigned char[]> load_tex(win::Stream &&stream, int &w, int &h);

	const std::chrono::time_point<std::chrono::high_resolution_clock> start = std::chrono::high_resolution_clock::now();

	win::Dimensions<int> res;
	win::Area<float> area;

	win::GLProgram program;
	int uniform_light;
	int uniform_lightcolor;
	int uniform_lightpower;
	int uniform_ftexcoord1_offset;
	int uniform_ftexcoord2_offset;
	int uniform_stretch;

	win::GLVertexArray vao;

	win::GLTexture background, texture;
};
