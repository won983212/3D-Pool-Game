#pragma once

class IButtonEvent
{
public:
	virtual void onButtonClick(int id) = 0;
};

class IScreenChangedEvent
{
public:
	virtual void onScreenChanged(int screenId) = 0;
};