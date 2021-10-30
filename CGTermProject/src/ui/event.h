#pragma once

class IButtonEvent
{
public:
	virtual void OnButtonClick(int id) = 0;
};

class IScreenChangedEvent
{
public:
	virtual void OnScreenChanged(int screen_id) = 0;
};
