#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "guiscreen.h"
#include "ui/button.h"
#include "util/util.h"

const clock_t MESSAGE_DURATION = 3 * CLOCKS_PER_SEC;
const float turnRectW = 200;


void GUIScreen::screenInit()
{
	const int btnYBase = 380;
	const int btnX = (int)((SCREEN_WIDTH - 300) / 2.0f);

	// 3 screens (menu, about, ingameHUD, game end)
	addPages(4);

	// menu screen
	// background panel
	UERectangle* rect = new UERectangle();
	rect->setBounds(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	rect->setColor(0x66ffffff);
	add(rect, 0);

	// title banner
	UEImage* img = new UEImage();
	img->setImage("res/texture/title.png");
	img->packSize();
	img->setLocation((SCREEN_WIDTH - img->width) / 2.0f, 80);
	add(img, 0);

	// buttons
	UEButton* btn = new UEButton();
	btn->id = 0;
	btn->setBounds(btnX, btnYBase, 300, 50);
	btn->setText(L"게임 시작");
	btn->setButtonEvent(this);
	add(btn, 0);

	btn = new UEButton();
	btn->id = 1;
	btn->setBounds(btnX, btnYBase + 80, 300, 50);
	btn->setText(L"게임 방법");
	btn->setButtonEvent(this);
	add(btn, 0);

	btn = new UEButton();
	btn->id = 2;
	btn->setBounds(btnX, btnYBase + 160, 300, 50);
	btn->setText(L"종료");
	btn->setButtonEvent(this);
	add(btn, 0);


	// TODO howtoplay screen
	// background panel
	add(rect, 1);

	// Back button
	btn = new UEButton();
	btn->id = 3;
	btn->setBounds(btnX, btnYBase, 300, 50);
	btn->setText(L"Back");
	btn->setButtonEvent(this);
	add(btn, 1);


	// ingame HUD
	// FPS Indicator
	fpsLabel = new UELabel();
	fpsLabel->setLocation(10, 25);
	fpsLabel->setTextPoint(24);
	fpsLabel->setShadowColor(0xff000000);
	fpsLabel->setText(L"FPS: 60");
	add(fpsLabel, 2);

	messageLabel = new UELabel();
	messageLabel->setLocation(SCREEN_WIDTH / 2, 50);
	messageLabel->setTextPoint(36);
	messageLabel->setShadowColor(0xff000000);
	messageLabel->setColor(0xffffffff);
	messageLabel->setUseCentered(true);
	messageLabel->setVisible(false);
	messageLabel->setText(L"");
	add(messageLabel, 2);

	turnLabel = new UELabel();
	turnLabel->setLocation(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 50);
	turnLabel->setTextPoint(24);
	turnLabel->setShadowColor(0xff000000);
	turnLabel->setColor(0xffffffff);
	turnLabel->setUseCentered(true);
	turnLabel->setText(L"");
	add(turnLabel, 2);

	UERectangle* turnQuadBG = new UERectangle();
	turnQuadBG->setBounds((SCREEN_WIDTH - turnRectW) / 2.0f, SCREEN_HEIGHT - 30, turnRectW, 5);
	turnQuadBG->setColor(0xff666666);
	add(turnQuadBG, 2);

	turnQuad = new UERectangle();
	turnQuad->setBounds((SCREEN_WIDTH - turnRectW) / 2.0f, SCREEN_HEIGHT - 30, turnRectW, 5);
	turnQuad->setColor(0xffE34E46);
	add(turnQuad, 2);

	turnIcon = new UEImage();
	turnIcon->setImage("res/texture/ball_icon.png");
	turnIcon->setBounds(SCREEN_WIDTH / 2.0f - 55, SCREEN_HEIGHT - 56, 12, 12);
	add(turnIcon, 2);


	// game end screen
	// background panel
	add(rect, 3);

	gameEndLabel = new UELabel();
	gameEndLabel->setLocation(SCREEN_WIDTH / 2, btnYBase - 80);
	gameEndLabel->setTextPoint(36);
	gameEndLabel->setShadowColor(0xff000000);
	gameEndLabel->setColor(0xffffffff);
	gameEndLabel->setUseCentered(true);
	add(gameEndLabel, 3);

	// main menu button
	btn = new UEButton();
	btn->id = 3;
	btn->setBounds(btnX, btnYBase, 300, 50);
	btn->setText(L"메인 메뉴로");
	btn->setButtonEvent(this);
	add(btn, 3);
}

void GUIScreen::goGameEnd(std::wstring message)
{
	gameEndLabel->setText(message);
	setScreen(3);
}

void GUIScreen::showMessage(std::wstring message)
{
	messageLabel->setText(message);
	lastMessageTick = clock();
}

void GUIScreen::setTurn(bool turn, BallGroup group, float achieve)
{
	turnIcon->setVisible(group != BallGroup::NOT_DECIDED);
	if (turn)
	{
		turnIcon->setUV(group == BallGroup::P1SOLID ? 0 : 24, 0, 24, 24);
		turnLabel->setText(L"Player 1");
		turnQuad->width = achieve * turnRectW;
	}
	else
	{
		turnIcon->setUV(group == BallGroup::P1SOLID ? 24 : 0, 0, 24, 24);
		turnLabel->setText(L"Player 2");
		turnQuad->width = achieve * turnRectW;
	}
}

void GUIScreen::onRenderTick()
{
	if (lastMessageTick != 0)
	{
		clock_t t = clock();
		messageLabel->setVisible(t - lastMessageTick < MESSAGE_DURATION);
	}
}

void GUIScreen::onButtonClick(int id)
{
	switch (id)
	{
	case 0:
		setScreen(2);
		break;
	case 1:
		setScreen(1);
		break;
	case 2:
		glutLeaveMainLoop();
		break;
	case 3:
		setScreen(0);
		break;
	}
}