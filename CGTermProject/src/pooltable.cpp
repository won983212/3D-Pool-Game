#include <iostream>
#include <string>
#include <glm/gtx/norm.hpp>
#include "pooltable.h"
#include "util/util.h"
#include "util/sound.h"


struct StaticWall
{
	glm::vec2 position;
	glm::vec2 normal;
	float size;
};


const float SQ3 = 1.73205080756f;
const float INV_SQ2 = 0.70710678118f;
const float CORNER_REDUCE = TABLE_HOLE_SIZE * INV_SQ2 + TABLE_WALL_THICKNESS;
const float SIDE_CORNER_REDUCE = CORNER_REDUCE - TABLE_HOLE_SIZE * 0.5f;

// line walls.
const StaticWall WALLS[] =
{
	{{0, TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS}, {0, -1}, TABLE_WIDTH - 2 * CORNER_REDUCE},
	{{0, -TABLE_HEIGHT / 2 + TABLE_WALL_THICKNESS}, {0, 1}, TABLE_WIDTH - 2 * CORNER_REDUCE},
	{{-TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS, TABLE_HEIGHT / 4 - SIDE_CORNER_REDUCE / 2}, {1, 0}, TABLE_HEIGHT / 2 - TABLE_HOLE_SIZE - SIDE_CORNER_REDUCE},
	{{-TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS, -TABLE_HEIGHT / 4 + SIDE_CORNER_REDUCE / 2}, {1, 0}, TABLE_HEIGHT / 2 - TABLE_HOLE_SIZE - SIDE_CORNER_REDUCE},
	{{TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS, TABLE_HEIGHT / 4 - SIDE_CORNER_REDUCE / 2}, {-1, 0}, TABLE_HEIGHT / 2 - TABLE_HOLE_SIZE - SIDE_CORNER_REDUCE},
	{{TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS, -TABLE_HEIGHT / 4 + SIDE_CORNER_REDUCE / 2}, {-1, 0}, TABLE_HEIGHT / 2 - TABLE_HOLE_SIZE - SIDE_CORNER_REDUCE}
};

// holes.
const glm::vec2 HOLES[] =
{
	{-TABLE_WIDTH / 2, 0.0f},
	{TABLE_WIDTH / 2, 0.0f},
	{TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS, TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS},
	{TABLE_WIDTH / 2 - TABLE_WALL_THICKNESS, -TABLE_HEIGHT / 2 + TABLE_WALL_THICKNESS},
	{-TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS, TABLE_HEIGHT / 2 - TABLE_WALL_THICKNESS},
	{-TABLE_WIDTH / 2 + TABLE_WALL_THICKNESS, -TABLE_HEIGHT / 2 + TABLE_WALL_THICKNESS}
};

const float FRICTION_MODIFIER = 0.99f;
const int BALL_INDEXES[] = {1, 11, 3, 6, 8, 14, 13, 15, 4, 9, 7, 2, 10, 5, 12};


PoolTable::PoolTable()
{
	const float gap = TABLE_HEIGHT * 0.8f / 15.0f;
	for (int i = 0; i < 16; i++)
		addBall(0.0f, i * gap - TABLE_HEIGHT * 0.8f / 2.0f);
}

PoolTable::~PoolTable()
{
	for (unsigned int i = 0; i < balls.size(); i++)
		delete balls[i];
}

void PoolTable::resetBallPosition()
{
	// white(0) ball setup
	balls[0]->position = glm::vec2(0.0f, -TABLE_HEIGHT / 3);

	// 1~15 ball setup
	int i = 0;
	for (int row = 0; row < 5; row++)
	{
		for (int column = 0; column <= row; column++)
		{
			glm::vec2 location = glm::vec2(2 * column - row, row * SQ3) * BALL_RADIUS;
			location.y += TABLE_HEIGHT / 4;
			balls[BALL_INDEXES[i++]]->position = location;
		}
	}
}

void PoolTable::update(float partialTime)
{
	bool isAllStop = true;
	for (unsigned int i = 0; i < balls.size(); i++)
	{
		Ball* ball = balls[i];
		if (!ball->visible)
			continue;

		// update velocity
		float vel = glm::length(ball->velocity);
		if (vel > 0)
			ball->velocity *= FRICTION_MODIFIER; // friction

		// velocity가 너무 낮으면 정지
		if (vel * FRICTION_MODIFIER < 0.02)
		{
			ball->velocity = glm::vec2(0.0f);
			vel = 0;
		}
		else
		{
			isAllStop = false;
		}

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

		// in hole test
		const int holeSize = sizeof(HOLES) / sizeof(glm::vec2);
		unsigned int j;
		for (j = 0; j < holeSize; j++)
		{
			const float adjHoleSize = (TABLE_HOLE_SIZE + 0.2f) * 0.5f;
			if (glm::length2(HOLES[j] - ball->position) < adjHoleSize * adjHoleSize)
				break;
		}

		// if ball is in hole, hide it.
		if (j < holeSize)
		{
			ball->visible = false;
			ball->velocity = glm::vec2(0.0f);
			if (ballEvent != nullptr)
				ballEvent->onBallHoleIn(i);
			continue;
		}

		// collision check & resolving. 1회만 test하기 위해 범위는 0~i로 설정.
		for (unsigned int j = 0; j < i; j++)
		{
			float dx = balls[i]->position.x - balls[j]->position.x;
			float dy = balls[i]->position.y - balls[j]->position.y;

			// collision test success
			float penetration_sq = 4 * BALL_RADIUS * BALL_RADIUS - (dx * dx + dy * dy);
			if (penetration_sq > 0.0001f)
			{
				// apply impulse
				glm::vec2 jNorm = glm::normalize(balls[j]->position - balls[i]->position);
				float power = glm::dot(balls[j]->velocity - balls[i]->velocity, jNorm);
				glm::vec2 impulse = power * jNorm;
				balls[i]->velocity += impulse;
				balls[j]->velocity -= impulse;

				// position correction
				float penetration = 2 * BALL_RADIUS - sqrt(dx * dx + dy * dy);
				balls[i]->position += penetration / 2 * -jNorm;
				balls[j]->position += penetration / 2 * jNorm;

				// play sound
				power = std::abs(power);
				int powerLevel = static_cast<int>(power / 2.0f);
				irrklang::ISound* sound = getSoundEngine()->play2D(("res/sound/ball_" + std::to_string(std::min(powerLevel, 2)) + ".wav").c_str(), false, false, true);
				sound->setVolume(glm::clamp((power - powerLevel * 2.0f), 0.3f, 1.0f));
				sound->drop();
			}
		}

		// wall collision test
		const int wallSize = sizeof(WALLS) / sizeof(StaticWall);
		for (unsigned int j = 0; j < wallSize; j++)
		{
			glm::vec2 diff = ball->position - WALLS[j].position;
			glm::vec2 wallN = glm::normalize(WALLS[j].normal);
			float dist = glm::dot(diff, wallN);
			if (dist < BALL_RADIUS)
			{
				glm::vec3 right3 = glm::cross(glm::vec3(wallN.x, 0, wallN.y), glm::vec3(0, 1, 0));
				glm::vec2 right = glm::normalize(glm::vec2(right3.x, right3.z));
				if (std::abs(glm::dot(right, diff)) * 2 < WALLS[j].size)
				{
					ball->position += (BALL_RADIUS - dist) * wallN;
					ball->velocity = glm::reflect(ball->velocity, wallN);
				}
			}
		}
	}

	if (isAllStop)
	{
		if (!stopEventCalled)
		{
			if (ballEvent != nullptr)
				ballEvent->onAllBallStopped();
			stopEventCalled = true;
		}
	}
	else
	{
		stopEventCalled = false;
	}
}

Ball* PoolTable::addBall(float x, float y)
{
	Ball* ball = new Ball();
	ball->position.x = x;
	ball->position.y = y;
	balls.push_back(ball);
	return ball;
}