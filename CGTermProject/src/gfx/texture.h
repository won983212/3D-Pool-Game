#pragma once
#include "gfx.h"

namespace commoncg
{
	class Texture
	{
	public:
		Texture(GLenum activeTexture = GL_TEXTURE0) 
			: active(activeTexture) {};
		void loadImage(const char* imageFilePath);
		void bind() const;
		void destroy();
		GLuint getTextureID() const;
		static void unbind();
	private:
		GLenum active;
		GLuint textureId = 0;
		int width = 0, height = 0, numChannels = 0;
	};
}