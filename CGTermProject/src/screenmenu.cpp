#include <iostream>
#include "screenmenu.h"
#include "ui/rectangle.h"
#include "ui/button.h"
#include "ui/image.h"
#include "util/util.h"

void ScreenMenu::init()
{
	UIScreen::init();

	const int btnYBase = 380;
	const int btnX = (SCREEN_WIDTH - 300) / 2.0f;

	// background panel
	UERectangle* rect = new UERectangle();
	rect->setBounds(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	rect->setColor(0x66ffffff);
	add(rect);

	// title banner
	UEImage* img = new UEImage();
	img->setImage("res/texture/title.png");
	img->packSize();
	img->setLocation((SCREEN_WIDTH - img->width) / 2.0f, 80);
	add(img);

	// buttons
	UEButton* btn = new UEButton();
	btn->id = 0;
	btn->setBounds(btnX, btnYBase, 300, 50);
	btn->setText(L"Start game");
	add(btn);

	btn = new UEButton();
	btn->id = 1;
	btn->setBounds(btnX, btnYBase + 80, 300, 50);
	btn->setText(L"About");
	add(btn);

	btn = new UEButton();
	btn->id = 2;
	btn->setBounds(btnX, btnYBase + 160, 300, 50);
	btn->setText(L"Exit");
	add(btn);
}

void ScreenMenu::onButtonClick(int id)
{
	std::cout << "CALL " << id << std::endl;
}