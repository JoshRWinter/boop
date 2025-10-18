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
	explicit GLPostProcessingRenderer(win::AssetRoll &roll);

	void draw(GLuint mainfb, GLuint scratchfb, float fps);

private:
	win::GLVertexArray vao;

	struct
	{
		win::GLProgram program;
		GLint uniform_fade;
	} histfade;

	struct
	{
		win::GLProgram program;
		GLint uniform_main_texture;
		GLint uniform_history_texture;
		GLint uniform_blur_horizontal;
	} post;
};
