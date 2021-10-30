#pragma once
#include "gfx.h"

namespace commoncg
{
	class Texture
	{
	public:
		~Texture();
		void LoadTextureImage(const char* image_file_path, GLint wrap_param = GL_CLAMP_TO_EDGE, bool use_mipmap = false);
		void Bind(GLenum target = GL_TEXTURE_2D) const;
		void Destroy() const;
		GLuint GetTextureId() const;
		int GetWidth() const;
		int GetHeight() const;
		int GetCountChannels() const;
		int GetCountMipmap() const;
		static Texture* CacheImage(const char* image_file_path, GLint wrap_param = GL_CLAMP_TO_EDGE, bool use_mipmap = false);
		static void Unbind(GLenum target = GL_TEXTURE_2D);

	private:
		void LoadDDSImage(const char* image_file_path, GLint wrap_param = GL_REPEAT);

	private:
		GLuint texture_id_ = 0;
		int width_ = 0, height_ = 0, num_channels_ = 0, mipmap_levels_ = 1;
	};
}
