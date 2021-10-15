#pragma once

#include <vector>
#include "gfx/gfx.h"
#include "util/util.h"

class IBallEvent
{
public:
	virtual void onAllBallStopped() = 0;
	virtual void onBallHoleIn(int ballId) = 0;
};

class Ball
{
public:
	Ball() : position(0.0f), velocity(0.0f) 
	{
		rotation = glm::angleAxis(90.0f, glm::vec3(0, 1, 0));
	};
public:
	glm::vec2 position;
	glm::vec2 velocity;
	glm::quat rotation;
	bool visible = true;
	bool highlight = false;
};

class PoolTable
{
public:
	PoolTable();
	~PoolTable();
	void resetBallPosition();
	void update(float partialTime);
	bool canPlaceWhiteBall();

private:
	Ball* addBall(float x, float y);

public:
	void setBallEvent(IBallEvent* e)
	{
		ballEvent = e;
	}
	const std::vector<Ball*>& getBalls() const
	{
		return balls;
	}

private:
	std::vector<Ball*> balls;
	IBallEvent* ballEvent = nullptr;
	bool stopEventCalled = false;
};