#pragma once
#include <string>
#include "uielement.h"

class Label : public UIElement
{
public:
	virtual bool render(std::vector<UIVertex>& vertices);
	void setText(std::wstring text);
	void setColor(int color);
	void setUseCentered(bool centered);
	void setTextScale(float scale);
private:
	std::wstring text;
	int color = 0xffffffff;
	bool centered = false;
	float scale = 0.5;
};