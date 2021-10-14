#pragma once
#include "uielement.h"

class UERectangle : public UIElement
{
public:
	virtual bool render(std::vector<UIVertex>& vertices);
	void setColor(int color);
private:
	int color = 0xffffffff;
};