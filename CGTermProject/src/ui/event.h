#pragma once

class IButtonEvent
{
public:
	virtual void onButtonClick(int id) = 0;
};