#pragma once
#include "ui/uiscreen.h"
#include "ui/label.h"

class GUIScreen : public UIScreen
{
public:
	virtual void screenInit();
	virtual void onButtonClick(int id);
public:
	Label* fpsLabel;
};