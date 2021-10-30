#pragma once
#include <string>
#include "event.h"
#include "uielement.h"

class UEButton : public UIElement
{
public:
	virtual bool Render(std::vector<UIVertex>& vertices);
	virtual void PostRender();
	virtual bool OnMouse(int button, int state, int x, int y);
	virtual void OnMouseMove(int x, int y);
	void SetText(std::wstring text);
	void SetTextScale(float scale);
	void SetButtonEvent(IButtonEvent* e);

private:
	IButtonEvent* btn_event_ = nullptr;
	std::wstring text_;
	bool hover_ = false;
	float text_scale_ = 0.5f;
};
