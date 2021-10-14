#include "rectangle.h"

bool UERectangle::render(std::vector<UIVertex>& vertices)
{
	vertices.push_back(makeVertex(x, y, color));
	vertices.push_back(makeVertex(x + width, y, color));
	vertices.push_back(makeVertex(x, y + height, color));
	vertices.push_back(makeVertex(x + width, y, color));
	vertices.push_back(makeVertex(x + width, y + height, color));
	vertices.push_back(makeVertex(x, y + height, color));
	return false;
}

void UERectangle::setColor(int color)
{
	this->color = color;
}