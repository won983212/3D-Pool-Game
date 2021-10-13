#include <iostream>
#include "pooltable.h"
#include "util/util.h"


PoolTable::PoolTable(glm::vec2 size)
	: size(size)
{
	// TODO Initialze ball positions
	for (int i = 0; i < 16; i++)
		addBall(randomVal(-size.x / 2, size.x / 2), randomVal(-size.y / 2, size.y / 2));
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

		// update velocity
		float vel = glm::length(ball->velocity);
		// TODO (Debug) No apply friction
		//if (vel > 0)
			//ball->velocity *= 0.99; // friction

		// update position
		ball->position += partialTime * ball->velocity;

		// update rotation
		if (vel > 0)
		{
			glm::vec3 vel3d = glm::vec3(ball->velocity.x, 0, ball->velocity.y);
			glm::vec3 rotationAxis = glm::cross(glm::vec3(0, 1, 0), vel3d);
			float dAngle = partialTime * glm::length(ball->velocity) / BALL_RADIUS; // dt*|v|/r
			ball->rotation = glm::angleAxis(dAngle, glm::normalize(rotationAxis)) * ball->rotation;
		}

		// collision check & resolving. 1회만 test하기 위해 범위는 0~i로 설정.
		for (unsigned int j = 0; j < i; j++)
		{
			float dx = balls[i]->position.x - balls[j]->position.x;
			float dy = balls[i]->position.y - balls[j]->position.y;

			// collision test success
			float penetration_sq = 4 * BALL_RADIUS * BALL_RADIUS - (dx * dx + dy * dy);
			if (penetration_sq > 0)
			{
				// apply impulse
				glm::vec2 jNorm = glm::normalize(balls[j]->position - balls[i]->position);
				glm::vec2 impulse = glm::dot(balls[j]->velocity - balls[i]->velocity, jNorm) * jNorm;
				balls[i]->velocity += impulse;
				balls[j]->velocity -= impulse;

				// position correction
				float penetration = 2 * BALL_RADIUS - sqrt(dx * dx + dy * dy);
				balls[i]->position += penetration / 2 * -jNorm;
				balls[j]->position += penetration / 2 * jNorm;
			}
		}

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