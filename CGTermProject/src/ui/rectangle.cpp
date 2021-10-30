#include "rectangle.h"

bool UERectangle::Render(std::vector<UIVertex>& vertices)
{
	vertices.push_back(MakeVertex(x_, y_, color_));
	vertices.push_back(MakeVertex(x_ + width_, y_, color_));
	vertices.push_back(MakeVertex(x_, y_ + height_, color_));
	vertices.push_back(MakeVertex(x_ + width_, y_, color_));
	vertices.push_back(MakeVertex(x_ + width_, y_ + height_, color_));
	vertices.push_back(MakeVertex(x_, y_ + height_, color_));
	return false;
}

void UERectangle::SetColor(int color)
{
	this->color_ = color;
}
