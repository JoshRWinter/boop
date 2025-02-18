#pragma once

#include <win/Win.hpp>
#include <win/gl/GL.hpp>
#include <win/AssetRoll.hpp>
#include <win/Utility.hpp>

class GLPostProcessingRenderer
{
	WIN_NO_COPY_MOVE(GLPostProcessingRenderer);

public:
	GLPostProcessingRenderer(win::AssetRoll &roll, const win::Dimensions<int> &screenres);

	void draw(GLuint fb);

private:
	win::GLVertexArray vao;

	struct
	{
		win::GLProgram program;
	} histfade;

	struct
	{
		win::GLFramebuffer fbo;
		win::GLTexture tex;
		win::GLProgram program;
		int uniform_horizontal;
		int uniform_texture;
	} blur;

	struct
	{
		win::GLProgram program;
		GLint uniform_tex;
	} post;
};
