#include "uielement.h"

void UIElement::SetBounds(float x, float y, float width, float height)
{
	SetLocation(x, y);
	SetSize(width, height);
}

void UIElement::SetLocation(float x, float y)
{
	this->x_ = x;
	this->y_ = y;
}

void UIElement::SetSize(float width, float height)
{
	this->width_ = width;
	this->height_ = height;
}

void UIElement::SetVisible(bool visible)
{
	this->visible_ = visible;
}

UIVertex UIElement::MakeVertex(float x, float y, unsigned int color, float u, float v)
{
	float a = (color >> 24 & 0xff) / 255.0f;
	float r = (color >> 16 & 0xff) / 255.0f;
	float g = (color >> 8 & 0xff) / 255.0f;
	float b = (color & 0xff) / 255.0f;
	return {{x, y}, {u, v}, {r, g, b, a}};
}

UIVertex UIElement::MakeVertex(float x, float y, unsigned int color)
{
	return MakeVertex(x, y, color, 0, 0);
}

UIVertex UIElement::MakeVertex(float x, float y, float u, float v)
{
	return MakeVertex(x, y, 0xffffffff, u, v);
}
