#pragma once

#include <glm/glm.hpp>

#include <win/AssetRoll.hpp>
#include <win/gl/GL.hpp>
#include <win/gl/GLAtlas.hpp>
#include <win/Utility.hpp>

#include "../RendererBackend.hpp"

class GLRendererBackend : public RendererBackend
{
public:
	explicit GLRendererBackend(win::AssetRoll &roll, const win::Area<float> &area);
	~GLRendererBackend() override = default;

	void render(const std::vector<Renderable> &renderables) override;

private:
	static void generate_vertex_data(const win::Atlas &atlas, std::vector<float> &posdata, std::vector<unsigned short> &texcoord);

	glm::mat4 projection;

	win::GLAtlas atlas;
	win::GLProgram program;
	int uniform_transform;

	win::GLVertexArray vao;
	win::GLBuffer vbo_pos;
	win::GLBuffer vbo_texcoord;
};
