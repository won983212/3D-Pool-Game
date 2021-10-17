#pragma once
#include <string>
#include "ui/event.h"
#include "ui/uiscreen.h"
#include "ui/label.h"
#include "ui/rectangle.h"
#include "ui/image.h"
#include "gameenum.h"

class GUIScreen : public UIScreen, IButtonEvent
{
public:
	virtual void screenInit();
	virtual void onButtonClick(int id);
	virtual void onRenderTick();
	void showMessage(std::wstring message);
	void setTurn(bool turn, BallGroup group, float achieve);
	void goGameEnd(std::wstring message);
public:
	UELabel* fpsLabel;
	UELabel* messageLabel;
	UELabel* turnLabel;
	UERectangle* turnQuad;
	UEImage* turnIcon;
	UELabel* gameEndLabel;
	clock_t lastMessageTick = 0;
};