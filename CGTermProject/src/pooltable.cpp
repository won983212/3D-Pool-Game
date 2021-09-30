#include <iostream>
#include "pooltable.h"
#include "util/util.h"


PoolTable::PoolTable(glm::vec2 size)
	: size(size)
{
	for (int i = 0; i < 10; i++)
	{
		Ball* ball = addBall(randomVal(-size.x / 2, size.x / 2), randomVal(-size.y / 2, size.y / 2));
		ball->velocity.x = randomVal(-10, 10);
		ball->velocity.y = randomVal(-10, 10);
	}
}

PoolTable::~PoolTable()
{
	for (unsigned int i = 0; i < balls.size(); i++)
		delete balls[i];
}

void PoolTable::update(float partialTime) const
{
	for (unsigned int i = 0; i < balls.size(); i++)
	{
		Ball* ball = balls[i];
		glm::vec2 vel = ball->velocity;

		// update position
		ball->position += partialTime * vel;

		// update rotation
		glm::vec3 rotationAxis = glm::cross(glm::vec3(0, 1, 0), glm::vec3(vel.x, 0, vel.y));
		float dAngle = partialTime * glm::length(vel) / BALL_RADIUS;
		ball->rotation = glm::angleAxis(dAngle, glm::normalize(rotationAxis)) * ball->rotation;

		if (ball->position.x < -size.x / 2 + BALL_RADIUS)
		{
			ball->position.x = -size.x / 2 + BALL_RADIUS;
			ball->velocity.x *= -1;
		}
		else if (ball->position.x > size.x / 2 - BALL_RADIUS)
		{
			ball->position.x = size.x / 2 - BALL_RADIUS;
			ball->velocity.x *= -1;
		}
		else if (ball->position.y < -size.y / 2 + BALL_RADIUS)
		{
			ball->position.y = -size.y / 2 + BALL_RADIUS;
			ball->velocity.y *= -1;
		}
		else if (ball->position.y > size.y / 2 - BALL_RADIUS)
		{
			ball->position.y = size.y / 2 - BALL_RADIUS;
			ball->velocity.y *= -1;
		}
	}
}

const std::vector<Ball*>& PoolTable::getBalls() const
{
	return balls;
}

Ball* PoolTable::addBall(float x, float y)
{
	Ball* ball = new Ball();
	ball->position.x = x;
	ball->position.y = y;
	balls.push_back(ball);
	return ball;
}