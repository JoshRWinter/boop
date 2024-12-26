#include "Renderer.hpp"
#include "gl/GLRendererBackend.hpp"

Renderer::Renderer(win::AssetRoll &roll)
	: backend(new GLRendererBackend(roll))
{
}
