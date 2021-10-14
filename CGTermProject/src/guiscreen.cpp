#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "guiscreen.h"
#include "ui/rectangle.h"
#include "ui/button.h"
#include "ui/image.h"
#include "util/util.h"

const clock_t MESSAGE_DURATION = 3 * CLOCKS_PER_SEC;


void GUIScreen::screenInit()
{
	const int btnYBase = 380;
	const int btnX = (int)((SCREEN_WIDTH - 300) / 2.0f);

	// 3 screens (menu, about, ingameHUD)
	addPages(3);

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
	fpsLabel = new Label();
	fpsLabel->setLocation(10, 25);
	fpsLabel->setTextPoint(24);
	fpsLabel->setShadowColor(0xff000000);
	fpsLabel->setText(L"FPS: 60");
	add(fpsLabel, 2);

	messageLabel = new Label();
	messageLabel->setLocation(SCREEN_WIDTH / 2, 50);
	messageLabel->setTextPoint(36);
	messageLabel->setShadowColor(0xff000000);
	messageLabel->setColor(0xffffffff);
	messageLabel->setUseCentered(true);
	messageLabel->setVisible(false);
	messageLabel->setText(L"");
	add(messageLabel, 2);
}

void GUIScreen::showMessage(std::wstring message)
{
	messageLabel->setText(message);
	lastMessageTick = clock();
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