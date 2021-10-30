#pragma once
#include "uielement.h"

class UERectangle : public UIElement
{
public:
	bool Render(std::vector<UIVertex>& vertices) override;
	void SetColor(int color);

private:
	unsigned int color_ = 0xffffffff;
};
