#include <iostream>
#include "screenmenu.h"
#include "button.h"
#include "image.h"
#include "label.h"

void ScreenMenu::init()
{
	UIScreen::init();

	Button* btn = new Button();
	btn->id = 0;
	btn->setBounds(200, 200, 200, 40);
	btn->setText(L"Hello, world!");
	btn->setTextScale(0.5f);
	add(btn);

	Image* img = new Image();
	img->setLocation(400, 200);
	img->setImage("res/texture/title.png");
	img->packSize();
	add(img);

	Label* label = new Label();
	label->setLocation(200, 300);
	label->setText(L"Nice to meet you Æ÷ÄÏº¼");
	add(label);
}

void ScreenMenu::onButtonClick(int id)
{
	std::cout << "CALL " << id << std::endl;
}