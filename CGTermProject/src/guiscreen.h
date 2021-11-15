#pragma once
#include <string>
#include "ui/event.h"
#include "ui/uiscreen.h"
#include "ui/label.h"
#include "ui/rectangle.h"
#include "ui/image.h"
#include "gameenum.h"

enum class ScreenPage
{
	Loading,
	Menu,
	About1,
	About2,
	InGame,
	GameEnd
};

class GuiScreen : public UIScreen, IButtonEvent
{
public:
	virtual void ScreenInit();
	virtual void OnButtonClick(int id);
	virtual void OnRenderTick();
	void ShowMessage(std::wstring message);
	void SetTurn(bool turn, BallGroup group, float achieve) const;
	void GoGameEnd(std::wstring message);
	void SetPage(ScreenPage page);
	ScreenPage GetCurrentPage() const;
	void AddToPage(UIElement* element, ScreenPage page);

public:
	UELabel* fps_label_;
	UELabel* message_label_;
	UELabel* turn_label_;
	UERectangle* turn_quad_;
	UEImage* turn_icon_;
	UELabel* game_end_label_;
	clock_t last_message_tick_ = 0;
};
