#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <string>
#include "texture.h"
#include "../3rd/stb_image.h"

using namespace commoncg;

// loaded plane texture cache
std::unordered_map<std::string, Texture*> loaded_textures;


Texture::~Texture()
{
	Destroy();
}

Texture* Texture::CacheImage(const char* image_file_path, const GLint wrap_param, const bool use_mipmap)
{
	std::string str_s(image_file_path);
	if (loaded_textures.find(str_s) != loaded_textures.end())
		return loaded_textures[str_s];

	auto texture = new Texture();
	texture->LoadTextureImage(image_file_path, wrap_param, use_mipmap);

	loaded_textures.insert(std::make_pair(str_s, texture));
	return texture;
}

void Texture::LoadTextureImage(const char* image_file_path, const GLint wrap_param, const bool use_mipmap)
{
	bool hdr = false;
	auto ext = std::string(image_file_path);
	ext = ext.substr(ext.find_last_of('.') + 1);

	// to lower
	std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower);

	if (ext == "dds")
	{
		LoadDdsImage(image_file_path, wrap_param);
		return;
	}
	if (ext == "hdr")
	{
		hdr = true;
	}

	glGenTextures(1, &texture_id_);
	if (texture_id_ == 0)
	{
		std::cout << "Failed to allocate memory for texture" << std::endl;
		return;
	}
	Bind();

	// setup parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, use_mipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load texture
	stbi_set_flip_vertically_on_load(hdr);
	void* data;
	if (hdr)
	{
		data = stbi_loadf(image_file_path, &width_, &height_, &num_channels_, 0);
	}
	else
	{
		data = stbi_load(image_file_path, &width_, &height_, &num_channels_, STBI_rgb_alpha);
	}

	if (data)
	{
		const GLint internal_format = hdr ? GL_RGB16F : GL_RGBA;
		const GLenum format = hdr ? GL_RGB : GL_RGBA;
		const GLenum type = hdr ? GL_FLOAT : GL_UNSIGNED_BYTE;
		glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width_, height_, 0, format, type, data);
		if (use_mipmap)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << image_file_path << " (HDR: " << hdr << ")" << std::endl;
	}
	stbi_image_free(data);
}

static void HandleError(const std::string& message, void* header, void* buffer, FILE* file)
{
	std::cout << "DDSLoader Error: " << message << std::endl;
	if (header != nullptr)
		free(header);
	if (buffer != nullptr)
		free(buffer);
	fclose(file);
}

// Reference: https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673
void Texture::LoadDdsImage(const char* image_file_path, GLint wrap_param)
{
	unsigned char* header;
	unsigned int block_size, format;
	unsigned char* buffer = nullptr;

	FILE* f;
	if ((f = fopen(image_file_path, "rb")) == nullptr)
		return;

	fseek(f, 0, SEEK_END);
	const long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	header = static_cast<unsigned char*>(malloc(128)); // filecode + header = 128 bytes
	if (header == nullptr)
	{
		HandleError("Can't allocate *header", header, buffer, f);
		return;
	}

	fread(header, 1, 128, f);

	if (memcmp(header, "DDS ", 4) != 0)
	{
		HandleError("Invaild DDS File", header, buffer, f);
		return;
	}

	height_ = header[12] | header[13] << 8 | header[14] << 16 | header[15] << 24;
	width_ = header[16] | header[17] << 8 | header[18] << 16 | header[19] << 24;
	mipmap_levels_ = header[28] | header[29] << 8 | header[30] << 16 | header[31] << 24;

	if (header[84] == 'D')
	{
		switch (header[87])
		{
		case '1': // DXT1
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			block_size = 8;
			break;
		case '3': // DXT3
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			block_size = 16;
			break;
		case '5': // DXT5
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			block_size = 16;
			break;
		default:
			HandleError(std::string("Unsupported Format") + static_cast<char>(header[87]), header, buffer, f);
			return;
		}
	}
	else
	{
		HandleError(std::string("Unsupported Format") + static_cast<char>(header[84]), header, buffer, f);
		return;
	}

	buffer = static_cast<unsigned char*>(malloc(file_size - 128));
	if (buffer == nullptr)
	{
		HandleError("Can't allocate *buffer", header, buffer, f);
		return;
	}

	fread(buffer, 1, file_size, f);

	glGenTextures(1, &texture_id_);
	if (texture_id_ == 0)
	{
		HandleError("Can't allocate texture", header, buffer, f);
		return;
	}

	Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmap_levels_ - 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_param);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_param);

	unsigned int offset = 0;
	unsigned int size = 0;
	unsigned int w = width_;
	unsigned int h = height_;

	for (int i = 0; i < mipmap_levels_; i++)
	{
		if (w == 0 || h == 0)
		{
			mipmap_levels_--;
			continue;
		}

		size = (w + 3) / 4 * ((h + 3) / 4) * block_size;
		glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
		offset += size;
		w /= 2;
		h /= 2;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmap_levels_ - 1);
	Unbind();
}

void Texture::Bind(GLenum target) const
{
	if (texture_id_ == 0)
	{
		std::cout << "Texture is not prepared!" << std::endl;
		return;
	}

	glBindTexture(target, texture_id_);
}

void Texture::Destroy()
{
	if (texture_id_ == 0)
	{
		std::cout << "Texture is not prepared!" << std::endl;
		return;
	}

	glDeleteTextures(1, &texture_id_);
}

void Texture::Unbind(GLenum target)
{
	glBindTexture(target, 0);
}

GLuint Texture::GetTextureId() const
{
	return texture_id_;
}

int Texture::GetWidth() const
{
	return width_;
}

int Texture::GetHeight() const
{
	return height_;
}

int Texture::GetCountChannels() const
{
	return num_channels_;
}

int Texture::GetCountMipmap() const
{
	return mipmap_levels_;
}
