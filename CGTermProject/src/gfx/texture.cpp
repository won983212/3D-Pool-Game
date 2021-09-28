#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <unordered_map>
#include "texture.h"
#include "../util/util.h"
#include "../util/stb_image.h"

std::unordered_map<std::string, Texture> loaded_textures;


Texture::~Texture()
{
	destroy();
}

Texture Texture::cacheImage(const char* imageFilePath)
{
	std::string str_s(imageFilePath);
	if (loaded_textures.find(str_s) != loaded_textures.end())
	{
		std::cout << "Cached: " << str_s << std::endl;
		return loaded_textures[str_s];
	}

	commoncg::Texture texture;
	texture.loadImage(imageFilePath);

	loaded_textures.insert(std::make_pair(str_s, texture));
	std::cout << "Loaded: " << str_s << std::endl;
	return texture;
}

void Texture::loadImage(const char* imageFilePath)
{
	// generate texture
	glGenTextures(1, &textureId);
	bind();

	// setup parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load texture
	unsigned char* data = stbi_load(imageFilePath, &width, &height, &numChannels, STBI_rgb_alpha);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "Failed to load texture: " << imageFilePath << std::endl;
	}

	unbind();
	if(data) stbi_image_free(data);
}

void Texture::bind() const
{
	if (textureId == 0)
	{
		std::cout << "Texture is not prepared!" << std::endl;
		return;
	}

	glBindTexture(GL_TEXTURE_2D, textureId);
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

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint Texture::getTextureID() const
{
	return textureId;
}