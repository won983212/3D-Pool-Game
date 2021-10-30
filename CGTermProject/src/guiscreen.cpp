#include <iostream>
#include <utility>
#include <ctime>
#include "guiscreen.h"
#include "ui/button.h"
#include "util/util.h"

constexpr clock_t MessageDuration = 3 * CLOCKS_PER_SEC;
constexpr float TurnRectW = 200;


void GuiScreen::ScreenInit()
{
	constexpr int btn_y_base = 380;
	constexpr int btn_x = static_cast<int>((SCREEN_WIDTH - 300) / 2.0f);

	// 3 screens (menu, about, ingameHUD, game end)
	AddPages(4);

	// menu screen
	// background panel
	const auto rect = new UERectangle();
	rect->SetBounds(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	rect->SetColor(0x66ffffff);
	Add(rect, 0);

	// title banner
	const auto img = new UEImage();
	img->SetImage("res/texture/title.png");
	img->PackSize();
	img->SetLocation((SCREEN_WIDTH - img->width_) / 2.0f, 80);
	Add(img, 0);

	// buttons
	auto btn = new UEButton();
	btn->id_ = 0;
	btn->SetBounds(btn_x, btn_y_base, 300, 50);
	btn->SetText(L"게임 시작");
	btn->SetButtonEvent(this);
	Add(btn, 0);

	btn = new UEButton();
	btn->id_ = 1;
	btn->SetBounds(btn_x, btn_y_base + 80, 300, 50);
	btn->SetText(L"게임 방법");
	btn->SetButtonEvent(this);
	Add(btn, 0);

	btn = new UEButton();
	btn->id_ = 2;
	btn->SetBounds(btn_x, btn_y_base + 160, 300, 50);
	btn->SetText(L"종료");
	btn->SetButtonEvent(this);
	Add(btn, 0);


	// TODO howtoplay screen
	// background panel
	Add(rect, 1);

	// Back button
	btn = new UEButton();
	btn->id_ = 3;
	btn->SetBounds(btn_x, btn_y_base, 300, 50);
	btn->SetText(L"Back");
	btn->SetButtonEvent(this);
	Add(btn, 1);


	// ingame HUD
	// FPS Indicator
	fps_label_ = new UELabel();
	fps_label_->SetLocation(10, 25);
	fps_label_->SetTextPoint(24);
	fps_label_->SetShadowColor(0xff000000);
	fps_label_->SetText(L"FPS: 60");
	Add(fps_label_, 2);

	message_label_ = new UELabel();
	message_label_->SetLocation(SCREEN_WIDTH / 2, 50);
	message_label_->SetTextPoint(36);
	message_label_->SetShadowColor(0xff000000);
	message_label_->SetColor(0xffffffff);
	message_label_->SetUseCentered(true);
	message_label_->SetVisible(false);
	message_label_->SetText(L"");
	Add(message_label_, 2);

	turn_label_ = new UELabel();
	turn_label_->SetLocation(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50);
	turn_label_->SetTextPoint(24);
	turn_label_->SetShadowColor(0xff000000);
	turn_label_->SetColor(0xffffffff);
	turn_label_->SetUseCentered(true);
	turn_label_->SetText(L"");
	Add(turn_label_, 2);

	const auto turn_quad_bg = new UERectangle();
	turn_quad_bg->SetBounds((SCREEN_WIDTH - TurnRectW) / 2.0f, SCREEN_HEIGHT - 30, TurnRectW, 5);
	turn_quad_bg->SetColor(0xff666666);
	Add(turn_quad_bg, 2);

	turn_quad_ = new UERectangle();
	turn_quad_->SetBounds((SCREEN_WIDTH - TurnRectW) / 2.0f, SCREEN_HEIGHT - 30, TurnRectW, 5);
	turn_quad_->SetColor(0xffE34E46);
	Add(turn_quad_, 2);

	turn_icon_ = new UEImage();
	turn_icon_->SetImage("res/texture/ball_icon.png");
	turn_icon_->SetBounds(SCREEN_WIDTH / 2.0f - 55, SCREEN_HEIGHT - 56, 12, 12);
	Add(turn_icon_, 2);

	// game end screen
	// background panel
	Add(rect, 3);

	game_end_label_ = new UELabel();
	game_end_label_->SetLocation(SCREEN_WIDTH / 2, btn_y_base - 80);
	game_end_label_->SetTextPoint(36);
	game_end_label_->SetShadowColor(0xff000000);
	game_end_label_->SetColor(0xffffffff);
	game_end_label_->SetUseCentered(true);
	Add(game_end_label_, 3);

	// main menu button
	btn = new UEButton();
	btn->id_ = 3;
	btn->SetBounds(btn_x, btn_y_base, 300, 50);
	btn->SetText(L"메인 메뉴로");
	btn->SetButtonEvent(this);
	Add(btn, 3);
}

void GuiScreen::GoGameEnd(std::wstring message)
{
	game_end_label_->SetText(std::move(message));
	SetScreen(3);
}

void GuiScreen::ShowMessage(std::wstring message)
{
	message_label_->SetText(std::move(message));
	last_message_tick_ = clock();
}

void GuiScreen::SetTurn(bool turn, BallGroup group, float achieve) const
{
	turn_icon_->SetVisible(group != BallGroup::NotDecided);
	if (turn)
	{
		turn_icon_->SetUV(group == BallGroup::P1Solid ? 0 : 24, 0, 24, 24);
		turn_label_->SetText(L"Player 1");
		turn_quad_->width_ = achieve * TurnRectW;
	}
	else
	{
		turn_icon_->SetUV(group == BallGroup::P1Solid ? 24 : 0, 0, 24, 24);
		turn_label_->SetText(L"Player 2");
		turn_quad_->width_ = achieve * TurnRectW;
	}
}

void GuiScreen::OnRenderTick()
{
	if (last_message_tick_ != 0)
	{
		clock_t t = clock();
		message_label_->SetVisible(t - last_message_tick_ < MessageDuration);
	}
}

void GuiScreen::OnButtonClick(int id)
{
	switch (id)
	{
	case 0:
		SetScreen(2);
		break;
	case 1:
		SetScreen(1);
		break;
	case 2:
		glutLeaveMainLoop();
		break;
	case 3:
		SetScreen(0);
		break;
	default:
		std::cout << "Error: Invaild Button id: " << id << std::endl;
		break;
	}
}
