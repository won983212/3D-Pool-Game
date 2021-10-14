#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include "../gfx/gfx.h"
#include "../gfx/vao.h"
#include "../gfx/shader.h"

#define FONT_HEIGHT 40

struct Glyph
{
	unsigned int textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

struct FontHeight
{
	int aboveBase;
	int belowBase;
};

class FontRenderer
{
public:
	~FontRenderer();
	void init(const char* fontPath);
	void renderText(std::wstring text, float x, float y, int color = 0xffffffff, float pt = 24, bool centered = false);
	void renderText(commoncg::ShaderProgram& guiShader, std::wstring text, float x, float y, int color = 0xffffffff, float pt = 24, bool centered = false);
	int width(std::wstring text, float pt = 1);
	FontHeight maxHeight(std::wstring text, float pt = 1);
private:
	// return Glyph (textureID==0) if failed.
	bool cacheGlyph(FT_ULong c, Glyph* glyph);
private:
	bool libNeedInitialize = true;
	FT_Library ftLib;
	FT_Face face;
	std::map<FT_ULong, Glyph> glyphCache;
	commoncg::VAO vao;
	commoncg::VBO vbo;
};