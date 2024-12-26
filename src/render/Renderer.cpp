#include "Renderer.hpp"
#include "gl/GLRendererBackend.hpp"

Renderer::Renderer(win::AssetRoll &roll, const win::Area<float> &area)
	: backend(new GLRendererBackend(roll, area))
{
}
