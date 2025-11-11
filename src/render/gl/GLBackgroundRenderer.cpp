#include <cmath>

#include <win/Targa.hpp>

#include "GLConstants.hpp"
#include "GLBackgroundRenderer.hpp"

using namespace win::gl;

static int get_uniform(const win::GLProgram &program, const char *name)
{
	const auto loc = glGetUniformLocation(program.get(), name);

	if (loc == -1)
		win::bug("no uniform " + std::string(name));

	return loc;
}

GLBackgroundRenderer::GLBackgroundRenderer(win::AssetRoll &roll, const win::Dimensions<int> &res, const win::Area<float> &area)
	: res(res)
	, area(area)
{
	program = win::GLProgram(load_gl_shaders(roll["shader/gl/background.vert"], roll["shader/gl/background.frag"]));
	glUseProgram(program.get());

	const auto uniform_background_sampler = get_uniform(program, "background");
	const auto uniform_texture_sampler = get_uniform(program, "tex");
	uniform_light = get_uniform(program, "light");
	uniform_lightcolor = get_uniform(program, "lightcolor");
	uniform_lightpower = get_uniform(program, "lightpower");
	uniform_ftexcoord1_offset = get_uniform(program, "ftexcoord1_offset");
	uniform_ftexcoord2_offset = get_uniform(program, "ftexcoord2_offset");
	uniform_stretch = get_uniform(program, "stretch");

	glUniform1i(uniform_background_sampler, GLConstants::BACKGROUND_TEXTURE_UNIT - GL_TEXTURE0);
	glUniform1i(uniform_texture_sampler, GLConstants::TEXTURE_TEXTURE_UNIT - GL_TEXTURE0);

	glBindVertexArray(vao.get());

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glActiveTexture(GLConstants::BACKGROUND_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D_ARRAY, background.get());
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	{
		const char *backgrounds[] =
		{
			"texture/background0.tga",
			"texture/background1.tga",
			"texture/background2.tga"
		};

		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_SRGB8_ALPHA8, 1920, 1080, 3, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);

		int i = 0;
		for (const auto *const t : backgrounds)
		{
			int w, h;
			auto data = load_tex(roll[t], w, h);

			glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, w, h, 1, GL_BGRA, GL_UNSIGNED_BYTE, data.get());

			++i;
		}
	}

	glActiveTexture(GLConstants::TEXTURE_TEXTURE_UNIT);
	glBindTexture(GL_TEXTURE_2D, texture.get());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	{
		win::Targa tga(roll["texture/texture.tga"]);
		if (tga.bpp() != 24)
			win::bug("texture too fat");

		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, tga.width(), tga.height(), 0, GL_BGR, GL_UNSIGNED_BYTE, tga.data());
	}

	win::gl_check_error();
}

void GLBackgroundRenderer::set_res_area(const win::Dimensions<int> &res, const win::Area<float> &area)
{
	this->res = res;
	this->area = area;
}

void GLBackgroundRenderer::draw(const std::vector<LightRenderable> &lights)
{
	glUseProgram(program.get());
	glBindVertexArray(vao.get());

	if (lights.empty())
	{
		glUniform2f(uniform_light, 0.0f, 0.0f);
		glUniform3f(uniform_lightcolor, 0.0f, 0.0f, 0.0f);
		glUniform1f(uniform_lightpower, 0.0f);
	}
	else
	{
		int x, y;
		world_to_screen(lights[0].x, lights[0].y, x, y);
		glUniform2f(uniform_light, x, y);
		glUniform3f(uniform_lightcolor, lights[0].color.red, lights[0].color.green, lights[0].color.blue);
		glUniform1f(uniform_lightpower, (res.width / 10.0f) * lights[0].power);
	}

	const float seconds = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - start).count();
	glUniform2f(uniform_ftexcoord1_offset, -seconds * 0.002f, seconds * 0.005f);
	glUniform2f(uniform_ftexcoord2_offset, std::cosf(seconds / 8.0f) * 0.02f, std::sinf(seconds / 8.0f) * 0.02f);

	{
		//figure out background stretch
		const auto stretch = (area.right - 8.0f) / 8.0;
		glUniform1f(uniform_stretch, stretch);
	}

	glDrawArrays(GL_TRIANGLES, 0, 3);

	win::gl_check_error();
}

void GLBackgroundRenderer::world_to_screen(float x, float y, int &xi, int &yi)
{
	xi = ((x / (area.right - area.left)) * res.width) + (res.width / 2.0f);
	yi = ((y / (area.top - area.bottom)) * res.height) + (res.height / 2.0f);
}

std::unique_ptr<unsigned char[]> GLBackgroundRenderer::load_tex(win::Stream &&stream, int &w, int &h)
{
	win::Targa tga(std::move(stream));
	w = tga.width();
	h = tga.height();
	const auto source = tga.data();

	const auto len = w * h * 4;
	std::unique_ptr<unsigned char[]> mem(new unsigned char[len]);
	const auto dest = mem.get();

	if (tga.bpp() == 24)
	{
		const auto source_len = w * h * 3;
		for (int s = 0, d = 0; s < source_len; s += 3, d += 4)
		{
			dest[d + 0] = source[s + 0];
			dest[d + 1] = source[s + 1];
			dest[d + 2] = source[s + 2];
			dest[d + 3] = 255;
		}
	}
	else if (tga.bpp() == 32)
	{
		const auto to_linear = [](unsigned char c)
		{
			const float norm = c / 255.0f;
			return norm <= 0.04045f ? norm / 12.92f : std::powf((norm + 0.055f) / 1.055f, 2.4f);
		};

		const auto to_srgb = [](float f)
		{
			float c = f <= 0.0031308f ? 12.92f * f : ((1.055f * std::powf(f, 1.0f / 2.4f)) - 0.055f);
			return (int)std::roundf(c * 255);
		};

		for (int p = 0; p < len; p += 4)
		{
			const float b = to_linear(source[p + 0]);
			const float g = to_linear(source[p + 1]);
			const float r = to_linear(source[p + 2]);
			const float a = source[p + 3] / 255.0f;

			dest[p + 0] = to_srgb(b * a);
			dest[p + 1] = to_srgb(g * a);
			dest[p + 2] = to_srgb(r * a);
			dest[p + 3] = source[p + 3];
		}
	}

	return mem;
}
