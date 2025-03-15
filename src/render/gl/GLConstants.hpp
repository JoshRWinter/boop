#pragma once

#include <win/gl/GL.hpp>

class GLConstants
{
	GLConstants() = delete;

public:
	static constexpr GLenum ATLAS_TEXTURE_UNIT = GL_TEXTURE0;
	static constexpr GLenum MENU_ATLAS_TEXTURE_UNIT = GL_TEXTURE1;
	static constexpr GLenum FONT_TEXTURE_UNIT = GL_TEXTURE2;
	static constexpr GLenum MAIN_COLOR_ATTACHMENT_TEXTURE_UNIT = GL_TEXTURE3;
	static constexpr GLenum HISTORY_COLOR_ATTACHMENT_TEXTURE_UNIT = GL_TEXTURE4;
	static constexpr GLenum BLUR_COLOR_ATTACHMENT_TEXTURE_UNIT = GL_TEXTURE5;

	static constexpr GLuint TEXT_RENDERER_SHADER_STORAGE_BLOCK_BINDING = 0;
};
