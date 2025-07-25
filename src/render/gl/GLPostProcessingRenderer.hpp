#pragma once

#include <win/Win.hpp>
#include <win/gl/GL.hpp>
#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

#include "../Renderable.hpp"

class GLPostProcessingRenderer
{
	WIN_NO_COPY_MOVE(GLPostProcessingRenderer);

public:
	GLPostProcessingRenderer(win::AssetRoll &roll, const win::Dimensions<int> &screenres, const win::Area<float> &gamearea);

	void draw(GLuint fb, float fps);

private:
	win::Dimensions<int> screenres;
	win::Area<float> gamearea;

	win::GLVertexArray vao;

	struct
	{
		win::GLProgram program;
		GLint uniform_fade;
	} histfade;

	struct
	{
		win::GLFramebuffer fbo;
		win::GLTexture tex;
		win::GLProgram program;
		GLint uniform_main_texture;
		GLint uniform_history_texture;
		GLint uniform_blur_horizontal;
	} post;
};
