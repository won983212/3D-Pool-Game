#pragma once
#include "gfx.h"

namespace commoncg
{
	class Texture
	{
	public:
		static Texture cacheImage(const char* imageFilePath);
		void loadImage(const char* imageFilePath);
		void bind() const;
		void destroy();
		GLuint getTextureID() const;
		static void unbind();
	private:
		GLuint textureId = 0;
		int width = 0, height = 0, numChannels = 0;
	};
}