#include "image.h"

Image::~Image()
{
	if (texture != nullptr)
		delete texture;
}

bool Image::render(std::vector<UIVertex>& vertices)
{
	if (texture == nullptr)
		return false;

	vertices.push_back(makeVertex(x,		 y,			 0, 0));
	vertices.push_back(makeVertex(x + width, y,			 1, 0));
	vertices.push_back(makeVertex(x,		 y + height, 0, 1));
	vertices.push_back(makeVertex(x + width, y,			 1, 0));
	vertices.push_back(makeVertex(x + width, y + height, 1, 1));
	vertices.push_back(makeVertex(x,		 y + height, 0, 1));

	texture->bind();
	return true;
}

void Image::setImage(const char* imagePath)
{
	if (texture != nullptr)
		delete texture;
	texture = commoncg::Texture::cacheImage(imagePath);
}

void Image::packSize()
{
	if (texture == nullptr)
		return;
	width = texture->getWidth();
	height = texture->getHeight();
}