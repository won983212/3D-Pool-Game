#include "uielement.h"

void UIElement::setBounds(float x, float y, float width, float height)
{
	setLocation(x, y);
	setSize(width, height);
}

void UIElement::setLocation(float x, float y)
{
	this->x = x;
	this->y = y;
}

void UIElement::setSize(float width, float height)
{
	this->width = width;
	this->height = height;
}

void UIElement::setVisible(bool visible)
{
	this->visible = visible;
}

UIVertex UIElement::makeVertex(float x, float y, int color, float u, float v)
{
	float a = ((color >> 24) & 0xff) / 255.0f;
	float r = ((color >> 16) & 0xff) / 255.0f;
	float g = ((color >> 8) & 0xff) / 255.0f;
	float b = (color & 0xff) / 255.0f;
	return { {x, y}, {u, v}, {r, g, b, a} };
}

UIVertex UIElement::makeVertex(float x, float y, int color)
{
	return makeVertex(x, y, color, 0, 0);
}

UIVertex UIElement::makeVertex(float x, float y, float u, float v)
{
	return makeVertex(x, y, 0xffffffff, u, v);
}