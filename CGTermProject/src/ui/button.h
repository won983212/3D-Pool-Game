#pragma once
#include "uielement.h"

class Button : public UIElement
{
public:
	virtual bool render(std::vector<UIVertex>& vertices);
	virtual void postRender();
	virtual void onMouse(int button, int state, int x, int y);
	virtual void onMouseMove(int x, int y);
	void setText(const char* text);
	void setTextScale(float scale);
private:
	const char* text;
	bool hover = false;
	float textScale = 0.6f;
};