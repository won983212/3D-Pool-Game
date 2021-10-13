#pragma once
#include "gfx.h"

namespace commoncg
{
	class Texture
	{
	public:
		~Texture();
		void loadImage(const char* imageFilePath, GLint wrapParam = GL_CLAMP_TO_EDGE, bool useMipmap = false);
		void use(GLenum target = GL_TEXTURE_2D) const;
		void destroy();
		GLuint getTextureID() const;
		int getWidth() const;
		int getHeight() const;
		int getCountChannels() const;
		int getCountMipmap() const;
		static Texture* cacheImage(const char* imageFilePath, GLint wrapParam = GL_CLAMP_TO_EDGE, bool useMipmap = false);
		static void unbind(GLenum target = GL_TEXTURE_2D);
	private:
		void loadDDSImage(const char* imageFilePath, GLint wrapParam = GL_REPEAT);
	private:
		GLuint textureId = 0;
		int width = 0, height = 0, numChannels = 0, mipmapLevels = 1;
	};
}