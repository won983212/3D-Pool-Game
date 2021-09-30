#pragma once

#include <vector>
#include "gfx/gfx.h"
#include "util/util.h"

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
};

class PoolTable
{
public:
	PoolTable(glm::vec2 size);
	~PoolTable();
	void update(float partialTime) const;
	const std::vector<Ball*>& getBalls() const;
private:
	Ball* addBall(float x, float y);
private:
	std::vector<Ball*> balls;
	glm::vec2 size;
};