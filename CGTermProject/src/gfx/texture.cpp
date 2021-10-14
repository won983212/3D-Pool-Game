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
	destroy();
}

Texture* Texture::cacheImage(const char* imageFilePath, GLint wrapParam, bool useMipmap)
{
	std::string str_s(imageFilePath);
	if (loaded_textures.find(str_s) != loaded_textures.end())
		return loaded_textures[str_s];

	commoncg::Texture* texture = new Texture();
	texture->loadImage(imageFilePath, wrapParam, useMipmap);

	loaded_textures.insert(std::make_pair(str_s, texture));
	return texture;
}

void Texture::loadImage(const char* imageFilePath, GLint wrapParam, bool useMipmap)
{
	bool hdr = false;
	std::string ext = std::string(imageFilePath);
	ext = ext.substr(ext.find_last_of(".") + 1);

	// to lower
	std::transform(ext.begin(), ext.end(), ext.begin(), std::tolower);

	if (ext == "dds")
	{
		loadDDSImage(imageFilePath, wrapParam);
		return;
	}
	else if (ext == "hdr")
	{
		hdr = true;
	}

	glGenTextures(1, &textureId);
	if (textureId == 0)
	{
		std::cout << "Failed to allocate memory for texture" << std::endl;
		return;
	}
	bind();

	// setup parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load texture
	stbi_set_flip_vertically_on_load(hdr);
	void* data;
	if (hdr)
	{
		data = stbi_loadf(imageFilePath, &width, &height, &numChannels, 0);
	}
	else
	{
		data = stbi_load(imageFilePath, &width, &height, &numChannels, STBI_rgb_alpha);
	}

	if (data)
	{
		GLint internalFormat = hdr ? GL_RGB16F : GL_RGBA;
		GLenum format = hdr ? GL_RGB : GL_RGBA;
		GLenum type = hdr ? GL_FLOAT : GL_UNSIGNED_BYTE;
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);
		if (useMipmap)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture: " << imageFilePath << " (HDR: " << hdr << ")" << std::endl;
	}
	stbi_image_free(data);
}

static void handleError(std::string message, void* header, void* buffer, FILE* file)
{
	std::cout << "DDSLoader Error: " << message << std::endl;
	if (header != nullptr)
		free(header);
	if (buffer != nullptr)
		free(buffer);
	fclose(file);
}

// Reference: https://gist.github.com/tilkinsc/13191c0c1e5d6b25fbe79bbd2288a673
void Texture::loadDDSImage(const char* imageFilePath, GLint wrapParam)
{
	unsigned char* header;
	unsigned int blockSize, format;
	unsigned char* buffer = 0;

	FILE* f;
	if ((f = fopen(imageFilePath, "rb")) == 0)
		return;

	fseek(f, 0, SEEK_END);
	long file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	header = (unsigned char*)malloc(128); // filecode + header = 128 bytes
	if (header == 0)
	{
		handleError("Can't allocate *header", header, buffer, f);
		return;
	}

	fread(header, 1, 128, f);

	if (memcmp(header, "DDS ", 4) != 0)
	{
		handleError("Invaild DDS File", header, buffer, f);
		return;
	}

	height = (header[12]) | (header[13] << 8) | (header[14] << 16) | (header[15] << 24);
	width = (header[16]) | (header[17] << 8) | (header[18] << 16) | (header[19] << 24);
	mipmapLevels = (header[28]) | (header[29] << 8) | (header[30] << 16) | (header[31] << 24);

	if (header[84] == 'D')
	{
		switch (header[87])
		{
		case '1': // DXT1
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			blockSize = 8;
			break;
		case '3': // DXT3
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			blockSize = 16;
			break;
		case '5': // DXT5
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			blockSize = 16;
			break;
		default:
			handleError(std::string("Unsupported Format") + (char)header[87], header, buffer, f);
			return;
		}
	}
	else
	{
		handleError(std::string("Unsupported Format") + (char)header[84], header, buffer, f);
		return;
	}

	buffer = (unsigned char*)malloc(file_size - 128);
	if (buffer == 0)
	{
		handleError("Can't allocate *buffer", header, buffer, f);
		return;
	}

	fread(buffer, 1, file_size, f);

	glGenTextures(1, &textureId);
	if (textureId == 0)
	{
		handleError("Can't allocate texture", header, buffer, f);
		return;
	}

	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapLevels - 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapParam);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapParam);

	unsigned int offset = 0;
	unsigned int size = 0;
	unsigned int w = width;
	unsigned int h = height;

	for (int i = 0; i < mipmapLevels; i++)
	{
		if (w == 0 || h == 0)
		{
			mipmapLevels--;
			continue;
		}

		size = ((w + 3) / 4) * ((h + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, i, format, w, h, 0, size, buffer + offset);
		offset += size;
		w /= 2;
		h /= 2;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipmapLevels - 1);
	unbind();
}

void Texture::bind(GLenum target) const
{
	if (textureId == 0)
	{
		std::cout << "Texture is not prepared!" << std::endl;
		return;
	}

	glBindTexture(target, textureId);
}

void Texture::destroy()
{
	if (textureId == 0)
	{
		std::cout << "Texture is not prepared!" << std::endl;
		return;
	}

	glDeleteTextures(1, &textureId);
}

void Texture::unbind(GLenum target)
{
	glBindTexture(target, 0);
}

GLuint Texture::getTextureID() const
{
	return textureId;
}

int Texture::getWidth() const
{
	return width;
}

int Texture::getHeight() const
{
	return height;
}

int Texture::getCountChannels() const
{
	return numChannels;
}

int Texture::getCountMipmap() const
{
	return mipmapLevels;
}
