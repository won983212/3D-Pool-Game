#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include "../gfx/gfx.h"
#include "../gfx/vao.h"
#include "../gfx/shader.h"

#define FONT_HEIGHT 48

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
	void renderText(std::string text, float x, float y, int color = 0xffffffff, float scale = 1, bool centered = false);
	void renderText(commoncg::ShaderProgram& guiShader, std::string text, float x, float y, int color = 0xffffffff, float scale = 1, bool centered = false);
	int width(std::string text, float scale = 1);
	FontHeight maxHeight(std::string text, float scale = 1);
private:
	// return Glyph (textureID==0) if failed.
	bool cacheGlyph(char c, Glyph* glyph);
private:
	bool libNeedInitialize = true;
	FT_Library ftLib;
	FT_Face face;
	std::map<char, Glyph> glyphCache;
	commoncg::VAO vao;
	commoncg::VBO vbo;
};