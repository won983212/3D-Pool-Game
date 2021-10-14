#pragma once
#include <string>
#include "event.h"
#include "uielement.h"

class UEButton : public UIElement
{
public:
	virtual bool render(std::vector<UIVertex>& vertices);
	virtual void postRender();
	virtual bool onMouse(int button, int state, int x, int y);
	virtual void onMouseMove(int x, int y);
	void setText(std::wstring text);
	void setTextScale(float scale);
	void setButtonEvent(IButtonEvent* e);
private:
	IButtonEvent* ev = nullptr;
	std::wstring text;
	bool hover = false;
	float textScale = 0.5f;
};