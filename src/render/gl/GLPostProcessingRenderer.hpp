#pragma once

#include <win/gl/GL.hpp>
#include <win/AssetRoll.hpp>

#include <win/Win.hpp>

class GLPostProcessingRenderer
{
	WIN_NO_COPY_MOVE(GLPostProcessingRenderer);

public:
	GLPostProcessingRenderer(win::AssetRoll &roll);

	void draw(GLuint fb);

private:
	win::GLVertexArray vao;

	struct
	{
		win::GLProgram program;
	} histfade;

	struct
	{
		win::GLProgram program;
		GLint uniform_tex;
	} post;
};
