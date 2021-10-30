#include <iostream>
#include <utility>
#include "font.h"
#include "../util/util.h"
#include "uielement.h"

using namespace commoncg;

FontRenderer::~FontRenderer()
{
	if (!lib_need_initialize_)
	{
		FT_Done_Face(face_);
		FT_Done_FreeType(ft_lib_);
	}
}

void FontRenderer::Init(const char* font_path)
{
	if (lib_need_initialize_)
	{
		if (FT_Init_FreeType(&ft_lib_))
		{
			std::cout << "Error: Can't initialize FreeType" << std::endl;
			return;
		}

		if (FT_New_Face(ft_lib_, font_path, 0, &face_))
		{
			std::cout << "Error: Can't load a font: " << font_path << std::endl;
			return;
		}

		// Use width as 0: auto width (fits to height)
		FT_Set_Pixel_Sizes(face_, 0, FONT_HEIGHT);
		// set alignment 4 to 1 (because r채널 하나만 사용하므로)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		lib_need_initialize_ = false;
	}

	vao_.Create();
	vbo_.Create();
	vbo_.Buffer(SIZEOF(UIVertex, 6), NULL, GL_DYNAMIC_DRAW); // 6 vertices * (vec4(pos-2 + tex-2) + vec4-color)
	vao_.Attrib(0, 4, GL_FLOAT, sizeof(UIVertex), 0);
	vao_.Attrib(1, 4, GL_FLOAT, sizeof(UIVertex), offsetof(UIVertex, color));
	vbo_.Unbind();
	VAO::Unbind();
}

bool FontRenderer::CacheGlyph(FT_ULong c, Glyph* glyph)
{
	if (glyph_cache_.find(c) != glyph_cache_.end())
	{
		*glyph = glyph_cache_[c];
		return true;
	}

	if (lib_need_initialize_)
	{
		std::cout << "Error: Font::cacheGlyph need initializing" << std::endl;
		return false;
	}

	if (FT_Load_Char(face_, c, FT_LOAD_RENDER))
	{
		std::cout << "Error: Can't load glyph of " << c << std::endl;
		return false;
	}

	const unsigned int width = face_->glyph->bitmap.width;
	const unsigned int height = face_->glyph->bitmap.rows;
	const unsigned int offset_x = face_->glyph->bitmap_left;
	const unsigned int offset_y = face_->glyph->bitmap_top;
	const void* data = face_->glyph->bitmap.buffer;

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Use GL_RED format. (font bitmap data is grayscale)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glyph->texture_id = texture;
	glyph->size = glm::ivec2(width, height);
	glyph->bearing = glm::ivec2(offset_x, offset_y);
	glyph->advance = static_cast<unsigned int>(face_->glyph->advance.x);

	glyph_cache_.insert(std::pair<FT_ULong, Glyph>(c, *glyph));
	return true;
}

void FontRenderer::RenderText(std::wstring text, float x, float y, int color, float pt, bool centered)
{
	ShaderProgram::GetContextShader()->SetUniform("useFont", true);
	glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_ALBEDO);
	vao_.Use();

	if (centered)
	{
		const int w = Width(text, pt);
		const FontHeight h = MaxHeight(text, pt);
		x -= w / 2.0f;
		y -= (h.above_base + h.below_base) / 2.0f - h.above_base;
	}

	float scale = pt * 0.75f / static_cast<float>(FONT_HEIGHT);
	std::wstring::const_iterator c;
	for (c = text.begin(); c != text.end(); ++c)
	{
		Glyph glyph;
		if (!CacheGlyph(*c, &glyph))
			continue;

		const float width = glyph.size.x * scale;
		const float height = glyph.size.y * scale;
		const float pos_x = x + glyph.bearing.x * scale;
		const float pos_y = y - glyph.bearing.y * scale;

		const float a = (color >> 24 & 0xff) / 255.0f;
		const float r = (color >> 16 & 0xff) / 255.0f;
		const float g = (color >> 8 & 0xff) / 255.0f;
		const float b = (color & 0xff) / 255.0f;

		const float vertices[6][8] =
		{
			{pos_x, pos_y + height, 0.0f, 1.0f, r, g, b, a},
			{pos_x, pos_y, 0.0f, 0.0f, r, g, b, a},
			{pos_x + width, pos_y, 1.0f, 0.0f, r, g, b, a},

			{pos_x, pos_y + height, 0.0f, 1.0f, r, g, b, a},
			{pos_x + width, pos_y, 1.0f, 0.0f, r, g, b, a},
			{pos_x + width, pos_y + height, 1.0f, 1.0f, r, g, b, a}
		};

		glBindTexture(GL_TEXTURE_2D, glyph.texture_id);
		vbo_.SubBuffer(0, sizeof vertices, vertices);
		vbo_.Unbind();
		glDrawArrays(GL_TRIANGLES, 0, 6); // quad

		// unit of advance is 1/64 pixel. a >> 6 = a * 64
		x += (glyph.advance >> 6) * scale;
	}

	VAO::Unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
	ShaderProgram::GetContextShader()->SetUniform("useFont", false);
}

void FontRenderer::RenderText(const ShaderProgram& gui_shader, std::wstring text, float x, float y, int color, float pt, bool centered)
{
	ShaderProgram::Push();
	gui_shader.Use();
	RenderText(std::move(text), x, y, color, pt, centered);
	ShaderProgram::Pop();
}

int FontRenderer::Width(std::wstring text, float pt)
{
	int w = 0;
	for (auto c = text.begin(); c != text.end(); ++c)
	{
		Glyph glyph;
		if (!CacheGlyph(*c, &glyph))
			continue;
		w += glyph.advance >> 6;
	}
	return static_cast<int>(w * pt * 0.75f / static_cast<float>(FONT_HEIGHT));
}

FontHeight FontRenderer::MaxHeight(std::wstring text, float pt)
{
	int top_max = 0;
	int bottom_max = 0;
	for (auto c = text.begin(); c != text.end(); ++c)
	{
		Glyph glyph;
		if (!CacheGlyph(*c, &glyph))
			continue;
		if (top_max < glyph.bearing.y)
			top_max = glyph.bearing.y;
		if (bottom_max < glyph.size.y - glyph.bearing.y)
			bottom_max = glyph.size.y - glyph.bearing.y;
	}

	const float scale = pt * 0.75f / static_cast<float>(FONT_HEIGHT);
	return {static_cast<int>(top_max * scale), static_cast<int>(bottom_max * scale)};
}
