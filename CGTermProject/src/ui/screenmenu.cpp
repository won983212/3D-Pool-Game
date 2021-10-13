#include <iostream>
#include "screenmenu.h"
#include "button.h"

void ScreenMenu::init()
{
	UIScreen::init();

	Button* btn = new Button();
	btn->id = 0;
	btn->setBounds(200, 200, 200, 40);
	btn->setText("Hello, world!");
	btn->setTextScale(0.5f);
	add(btn);
}

void ScreenMenu::onButtonClick(int id)
{
	std::cout << "CALL " << id << std::endl;
}