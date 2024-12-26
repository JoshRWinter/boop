#include "GLRendererBackend.hpp"

GLRendererBackend::GLRendererBackend(win::AssetRoll &roll)
{

}

void GLRendererBackend::render(const std::vector<Renderable> &renderables)
{
	this->renderables = renderables;
}
