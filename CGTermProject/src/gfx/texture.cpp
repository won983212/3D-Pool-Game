#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include "texture.h"
#include "../util/util.h"
#include "../util/stb_image.h"


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

	glActiveTexture(active);
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