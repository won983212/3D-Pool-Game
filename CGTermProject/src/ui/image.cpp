#include "image.h"

UEImage::~UEImage()
{
	if (texture != nullptr)
		delete texture;
}

bool UEImage::render(std::vector<UIVertex>& vertices)
{
	if (texture == nullptr)
		return false;

	vertices.push_back(makeVertex(x, y, u1, v1));
	vertices.push_back(makeVertex(x + width, y, u2, v1));
	vertices.push_back(makeVertex(x, y + height, u1, v2));
	vertices.push_back(makeVertex(x + width, y, u2, v1));
	vertices.push_back(makeVertex(x + width, y + height, u2, v2));
	vertices.push_back(makeVertex(x, y + height, u1, v2));

	texture->bind();
	return true;
}

void UEImage::setImage(const char* imagePath)
{
	if (texture != nullptr)
		delete texture;
	texture = commoncg::Texture::cacheImage(imagePath);
}

void UEImage::setUV(int x, int y, int w, int h)
{
	u1 = (float)x / texture->getWidth();
	v1 = (float)y / texture->getHeight();
	u2 = (float)(x + w) / texture->getWidth();
	v2 = (float)(y + h) / texture->getHeight();
}

void UEImage::packSize()
{
	if (texture == nullptr)
		return;
	width = (float) texture->getWidth();
	height = (float) texture->getHeight();
}