#pragma once
#include <string>
#include "ui/event.h"
#include "ui/uiscreen.h"
#include "ui/label.h"

class GUIScreen : public UIScreen, IButtonEvent
{
public:
	virtual void screenInit();
	virtual void onButtonClick(int id);
	virtual void onRenderTick();
	void showMessage(std::wstring message);
public:
	Label* fpsLabel;
	Label* messageLabel;
	clock_t lastMessageTick = 0;
};