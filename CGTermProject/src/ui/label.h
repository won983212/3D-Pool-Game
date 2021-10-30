#pragma once
#include <string>
#include "uielement.h"

class UELabel : public UIElement
{
public:
	bool Render(std::vector<UIVertex>& vertices) override;
	void SetText(std::wstring text);
	void SetColor(unsigned int color);
	void SetShadowColor(unsigned int shadow_color);
	void SetUseCentered(bool centered);
	void SetTextPoint(float point);

private:
	std::wstring text_;
	unsigned int color_ = 0xffffffff;
	unsigned int shadow_color_ = 0;
	bool centered_ = false;
	float font_point_ = 24;
};
