#include <iostream>
#include "font.h"
#include "../util/util.h"
#include "uielement.h"

using namespace commoncg;

FontRenderer::~FontRenderer()
{
	if (!libNeedInitialize)
	{
		FT_Done_Face(face);
		FT_Done_FreeType(ftLib);
	}
}

void FontRenderer::init(const char* fontPath)
{
	if (libNeedInitialize)
	{
		if (FT_Init_FreeType(&ftLib))
		{
			std::cout << "Error: Can't initialze FreeType" << std::endl;
			return;
		}

		if (FT_New_Face(ftLib, fontPath, 0, &face))
		{
			std::cout << "Error: Can't load a font: " << fontPath << std::endl;
			return;
		}

		// use width as 0: auto width (fits to height)
		FT_Set_Pixel_Sizes(face, 0, FONT_HEIGHT);
		// set alignment 4 to 1 (because r채널 하나만 사용하므로)
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		libNeedInitialize = false;
	}

	vao.create();
	vbo.create();
	vbo.buffer(SIZEOF(UIVertex, 6), NULL, GL_DYNAMIC_DRAW); // 6 vertices * (vec4(pos-2 + tex-2) + vec4-color)
	vao.attr(0, 4, GL_FLOAT, sizeof(UIVertex), 0);
	vao.attr(1, 4, GL_FLOAT, sizeof(UIVertex), offsetof(UIVertex, color));
	vbo.unbind();
	VAO::unbind();
}

bool FontRenderer::cacheGlyph(FT_ULong c, Glyph* glyph)
{
	if (glyphCache.find(c) != glyphCache.end())
	{
		*glyph = glyphCache[c];
		return true;
	}

	if (libNeedInitialize)
	{
		std::cout << "Error: Font::cacheGlyph need initializing" << std::endl;
		return false;
	}

	if (FT_Load_Char(face, c, FT_LOAD_RENDER))
	{
		std::cout << "Error: Can't load glyph of " << c << std::endl;
		return false;
	}

	const unsigned int width = face->glyph->bitmap.width;
	const unsigned int height = face->glyph->bitmap.rows;
	const unsigned int offsetX = face->glyph->bitmap_left;
	const unsigned int offsetY = face->glyph->bitmap_top;
	const void* data = face->glyph->bitmap.buffer;

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// use GL_RED format. (font bitmap data is grayscale)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glyph->textureID = texture;
	glyph->size = glm::ivec2(width, height);
	glyph->bearing = glm::ivec2(offsetX, offsetY);
	glyph->advance = static_cast<unsigned int>(face->glyph->advance.x);

	glyphCache.insert(std::pair<FT_ULong, Glyph>(c, *glyph));
	return true;
}

void FontRenderer::renderText(std::wstring text, float x, float y, int color, float pt, bool centered)
{
	ShaderProgram::getContextShader()->setUniform("useFont", true);
	glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_ALBEDO);
	vao.use();

	if (centered)
	{
		const int w = width(text, pt);
		const FontHeight h = maxHeight(text, pt);
		x -= w / 2.0f;
		y -= (h.aboveBase + h.belowBase) / 2.0f - h.aboveBase;
	}

	float scale = pt * 0.75f / (float) FONT_HEIGHT;
	std::wstring::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Glyph glyph;
		if (!cacheGlyph(*c, &glyph))
			continue;

		float width = glyph.size.x * scale;
		float height = glyph.size.y * scale;
		float posX = x + glyph.bearing.x * scale;
		float posY = y - glyph.bearing.y * scale;

		float a = ((color >> 24) & 0xff) / 255.0f;
		float r = ((color >> 16) & 0xff) / 255.0f;
		float g = ((color >> 8) & 0xff) / 255.0f;
		float b = (color & 0xff) / 255.0f;

		float vertices[6][8] =
		{
			{ posX,			 posY + height,		0.0f, 1.0f, r, g, b, a },
			{ posX,			 posY,				0.0f, 0.0f, r, g, b, a },
			{ posX + width,	 posY,				1.0f, 0.0f, r, g, b, a },

			{ posX,			 posY + height,		0.0f, 1.0f, r, g, b, a },
			{ posX + width,	 posY,				1.0f, 0.0f, r, g, b, a },
			{ posX + width,	 posY + height,		1.0f, 1.0f, r, g, b, a }
		};

		glBindTexture(GL_TEXTURE_2D, glyph.textureID);
		vbo.subBuffer(0, sizeof(vertices), vertices);
		vbo.unbind();
		glDrawArrays(GL_TRIANGLES, 0, 6); // quad

		// unit of advance is 1/64 pixel. a >> 6 = a * 64
		x += (glyph.advance >> 6) * scale;
	}

	VAO::unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
	ShaderProgram::getContextShader()->setUniform("useFont", false);
}

void FontRenderer::renderText(ShaderProgram& guiShader, std::wstring text, float x, float y, int color, float pt, bool centered)
{
	ShaderProgram::push();
	guiShader.use();
	renderText(text, x, y, color, pt, centered);
	ShaderProgram::pop();
}

int FontRenderer::width(std::wstring text, float pt)
{
	int w = 0;
	std::wstring::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Glyph glyph;
		if (!cacheGlyph(*c, &glyph))
			continue;
		w += glyph.advance >> 6;
	}
	return (int)(w * pt * 0.75f / (float)FONT_HEIGHT);
}

FontHeight FontRenderer::maxHeight(std::wstring text, float pt)
{
	int topMax = 0;
	int bottomMax = 0;
	std::wstring::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Glyph glyph;
		if (!cacheGlyph(*c, &glyph))
			continue;
		if (topMax < glyph.bearing.y)
			topMax = glyph.bearing.y;
		if (bottomMax < (glyph.size.y - glyph.bearing.y))
			bottomMax = (glyph.size.y - glyph.bearing.y);
	}
	float scale = pt * 0.75f / (float)FONT_HEIGHT;
	return { (int)(topMax * scale), (int)(bottomMax * scale) };
}