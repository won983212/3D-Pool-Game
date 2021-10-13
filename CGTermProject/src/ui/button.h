#pragma once
#include <string>
#include "uielement.h"

class Button : public UIElement
{
public:
	virtual bool render(std::vector<UIVertex>& vertices);
	virtual void postRender();
	virtual void onMouse(int button, int state, int x, int y);
	virtual void onMouseMove(int x, int y);
	void setText(std::wstring text);
	void setTextScale(float scale);
private:
	std::wstring text;
	bool hover = false;
	float textScale = 0.6f;
};