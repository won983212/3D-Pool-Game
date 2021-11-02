#include <iostream>
#include <utility>
#include <ctime>
#include "guiscreen.h"
#include "ui/button.h"
#include "util/util.h"
#include "strings.h"

constexpr clock_t MessageDuration = 3 * CLOCKS_PER_SEC;
constexpr float TurnRectW = 200;


void GuiScreen::ScreenInit()
{
	constexpr int btn_y_base = 380;
	constexpr int btn_x = static_cast<int>((SCREEN_WIDTH - 300) / 2.0f);

	// add screens (loading, menu, about, ingameHUD, game end)
	AddPages(5);

	// loading screen
	// background panel
	const auto rect = new UERectangle();
	rect->SetBounds(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	rect->SetColor(0x66ffffff);
	AddToPage(rect, ScreenPage::Loading);

	// Loading text
	UELabel* loading_label = new UELabel();
	loading_label->SetLocation(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	loading_label->SetUseCentered(true);
	loading_label->SetTextPoint(64);
	loading_label->SetShadowColor(0xff000000);
	loading_label->SetText(L"리소스 로드중입니다...");
	AddToPage(loading_label, ScreenPage::Loading);


	// menu screen
	// background panel
	AddToPage(rect, ScreenPage::Menu);

	// title banner
	const auto img = new UEImage();
	img->SetImage("res/texture/title.png");
	img->PackSize();
	img->SetLocation((SCREEN_WIDTH - img->width_) / 2.0f, 80);
	AddToPage(img, ScreenPage::Menu);

	// buttons
	auto btn = new UEButton();
	btn->id_ = 0;
	btn->SetBounds(btn_x, btn_y_base, 300, 50);
	btn->SetText(TEXT_BTN_START_GAME);
	btn->SetButtonEvent(this);
	AddToPage(btn, ScreenPage::Menu);

	btn = new UEButton();
	btn->id_ = 1;
	btn->SetBounds(btn_x, btn_y_base + 80, 300, 50);
	btn->SetText(TEXT_BTN_HOW_TO_PLAY);
	btn->SetButtonEvent(this);
	AddToPage(btn, ScreenPage::Menu);

	btn = new UEButton();
	btn->id_ = 2;
	btn->SetBounds(btn_x, btn_y_base + 160, 300, 50);
	btn->SetText(TEXT_BTN_EXIT);
	btn->SetButtonEvent(this);
	AddToPage(btn, ScreenPage::Menu);


	// TODO howtoplay screen
	// background panel
	AddToPage(rect, ScreenPage::About);

	// Back button
	btn = new UEButton();
	btn->id_ = 3;
	btn->SetBounds(btn_x, btn_y_base, 300, 50);
	btn->SetText(TEXT_BTN_TO_MAIN);
	btn->SetButtonEvent(this);
	AddToPage(btn, ScreenPage::About);


	// ingame HUD
	// FPS Indicator
	fps_label_ = new UELabel();
	fps_label_->SetLocation(10, 25);
	fps_label_->SetTextPoint(24);
	fps_label_->SetShadowColor(0xff000000);
	fps_label_->SetText(L"FPS: 60");
	AddToPage(fps_label_, ScreenPage::InGame);

	message_label_ = new UELabel();
	message_label_->SetLocation(SCREEN_WIDTH / 2, 50);
	message_label_->SetTextPoint(36);
	message_label_->SetShadowColor(0xff000000);
	message_label_->SetColor(0xffffffff);
	message_label_->SetUseCentered(true);
	message_label_->SetVisible(false);
	message_label_->SetText(L"");
	AddToPage(message_label_, ScreenPage::InGame);

	turn_label_ = new UELabel();
	turn_label_->SetLocation(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50);
	turn_label_->SetTextPoint(24);
	turn_label_->SetShadowColor(0xff000000);
	turn_label_->SetColor(0xffffffff);
	turn_label_->SetUseCentered(true);
	turn_label_->SetText(L"");
	AddToPage(turn_label_, ScreenPage::InGame);

	const auto turn_quad_bg = new UERectangle();
	turn_quad_bg->SetBounds((SCREEN_WIDTH - TurnRectW) / 2.0f, SCREEN_HEIGHT - 30, TurnRectW, 5);
	turn_quad_bg->SetColor(0xff666666);
	AddToPage(turn_quad_bg, ScreenPage::InGame);

	turn_quad_ = new UERectangle();
	turn_quad_->SetBounds((SCREEN_WIDTH - TurnRectW) / 2.0f, SCREEN_HEIGHT - 30, TurnRectW, 5);
	turn_quad_->SetColor(0xffE34E46);
	AddToPage(turn_quad_, ScreenPage::InGame);

	turn_icon_ = new UEImage();
	turn_icon_->SetImage("res/texture/ball_icon.png");
	turn_icon_->SetBounds(SCREEN_WIDTH / 2.0f - 55, SCREEN_HEIGHT - 56, 12, 12);
	AddToPage(turn_icon_, ScreenPage::InGame);

	// game end screen
	// background panel
	AddToPage(rect, ScreenPage::GameEnd);

	game_end_label_ = new UELabel();
	game_end_label_->SetLocation(SCREEN_WIDTH / 2, btn_y_base - 80);
	game_end_label_->SetTextPoint(36);
	game_end_label_->SetShadowColor(0xff000000);
	game_end_label_->SetColor(0xffffffff);
	game_end_label_->SetUseCentered(true);
	AddToPage(game_end_label_, ScreenPage::GameEnd);

	// main menu button
	btn = new UEButton();
	btn->id_ = 3;
	btn->SetBounds(btn_x, btn_y_base, 300, 50);
	btn->SetText(TEXT_BTN_TO_MAIN);
	btn->SetButtonEvent(this);
	AddToPage(btn, ScreenPage::GameEnd);
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
		turn_label_->SetText(PLAYER1_NAME);
		turn_quad_->width_ = achieve * TurnRectW;
	}
	else
	{
		turn_icon_->SetUV(group == BallGroup::P1Solid ? 24 : 0, 0, 24, 24);
		turn_label_->SetText(PLAYER2_NAME);
		turn_quad_->width_ = achieve * TurnRectW;
	}
}

void GuiScreen::OnRenderTick()
{
	if (last_message_tick_ != 0)
	{
		const clock_t t = clock();
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
		std::cout << "Warning: Invaild Button id: " << id << std::endl;
		break;
	}
}

void GuiScreen::SetPage(ScreenPage page)
{
	SetScreen((int)page);
}

ScreenPage GuiScreen::GetCurrentPage() const
{
	return (ScreenPage)GetCurrentScreen();
}

void GuiScreen::AddToPage(UIElement* element, ScreenPage page)
{
	Add(element, (int)page);
}