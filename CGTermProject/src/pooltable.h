#pragma once

#include <vector>
#include "gfx/gfx.h"

class IBallEvent
{
public:
	virtual void OnAllBallStopped() = 0;
	virtual void OnBallHoleIn(int ball_id) = 0;
	virtual void OnWhiteBallCollide(int ball_id) = 0;
};

class Ball
{
public:
	Ball() : position_(0.0f), velocity_(0.0f) { rotation_ = angleAxis(90.0f, glm::vec3(0, 1, 0)); };

public:
	glm::vec2 position_;
	glm::vec2 velocity_;
	glm::quat rotation_;
	bool visible_ = true;
	bool highlight_ = false;
};

struct RaytraceResult
{
	bool hit;
	Ball* hit_target_ball;
	glm::vec2 hit_ball_pos;
	glm::vec2 normal;
};

class PoolTable
{
public:
	PoolTable();
	~PoolTable();
	void ResetBallPosition();
	void update(float partial_time);
	bool CanPlaceWhiteBall() const;
	RaytraceResult GetRaytracedBall(glm::vec2 pos, glm::vec2 dir) const;

private:
	Ball* AddBall(float x, float y);

public:
	void SetBallEvent(IBallEvent* e) { ball_event_ = e; }
	const std::vector<Ball*>& GetBalls() const { return balls_; }

private:
	std::vector<Ball*> balls_;
	IBallEvent* ball_event_ = nullptr;
	bool stop_event_called_ = true;
};
