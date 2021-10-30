#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include <string>
#include <map>
#include "../gfx/gfx.h"
#include "../gfx/vao.h"
#include "../gfx/vbo.h"
#include "../gfx/shader.h"

#define FONT_HEIGHT 40

struct Glyph
{
	unsigned int texture_id;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

struct FontHeight
{
	int above_base;
	int below_base;
};

class FontRenderer
{
public:
	~FontRenderer();
	void Init(const char* font_path);
	void RenderText(std::wstring text, float x, float y, int color = 0xffffffff, float pt = 24, bool centered = false);
	void RenderText(const commoncg::ShaderProgram& gui_shader, std::wstring text, float x, float y, int color = 0xffffffff, float pt = 24, bool centered = false);
	int Width(std::wstring text, float pt = 1);
	FontHeight MaxHeight(std::wstring text, float pt = 1);

private:
	// return Glyph (textureID==0) if failed.
	bool CacheGlyph(FT_ULong c, Glyph* glyph);

private:
	bool lib_need_initialize_ = true;
	FT_Library ft_lib_;
	FT_Face face_;
	std::map<FT_ULong, Glyph> glyph_cache_;
	commoncg::VAO vao_;
	commoncg::VBO vbo_;
};
