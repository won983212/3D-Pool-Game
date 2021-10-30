#pragma once
#include <string>
#include "ui/event.h"
#include "ui/uiscreen.h"
#include "ui/label.h"
#include "ui/rectangle.h"
#include "ui/image.h"
#include "gameenum.h"

class GuiScreen : public UIScreen, IButtonEvent
{
public:
	void ScreenInit() override;
	void OnButtonClick(int id) override;
	void OnRenderTick() override;
	void ShowMessage(std::wstring message);
	void SetTurn(bool turn, BallGroup group, float achieve) const;
	void GoGameEnd(std::wstring message);

public:
	UELabel* fps_label_;
	UELabel* message_label_;
	UELabel* turn_label_;
	UERectangle* turn_quad_;
	UEImage* turn_icon_;
	UELabel* game_end_label_;
	clock_t last_message_tick_ = 0;
};
