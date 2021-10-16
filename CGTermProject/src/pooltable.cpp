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
	for (int i = 0; i < BALL_COUNT; i++)
		addBall(0.0f, i * gap - TABLE_HEIGHT * 0.8f / 2.0f);
}

PoolTable::~PoolTable()
{
	for (unsigned int i = 0; i < balls.size(); i++)
		delete balls[i];
}

void PoolTable::resetBallPosition()
{
	/*// white(0) ball setup
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
	}*/
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
		glm::vec2 prevPos = ball->position;
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
		for (unsigned int j = 0; j < balls.size(); j++)
		{
			glm::vec2 delta = balls[j]->position - balls[i]->position;
			if (i == j || glm::dot(balls[i]->velocity, delta) < 0) // 정방향으로 충돌한 ball만 고려
				continue;

			// collision test
			float deltaSq = glm::length2(delta);
			float penetration_sq = 4 * BALL_RADIUS * BALL_RADIUS - deltaSq;
			if (penetration_sq > 0.0001f)
			{
				// discrete position correction (discrete하게 position을 update하므로 정확한 impulse적용을 위해 보정이 필요함)
				glm::vec2 vel = glm::normalize(balls[i]->velocity);
				glm::vec2 dPos = balls[j]->position - prevPos;
				float ph = glm::dot(vel, dPos);
				float lh = sqrt(4 * BALL_RADIUS * BALL_RADIUS - glm::length2(dPos) + ph * ph);

				balls[i]->position = prevPos + (ph - lh) * vel;
				delta = balls[j]->position - balls[i]->position;

				// apply impulse
				glm::vec2 jNorm = glm::normalize(delta);
				float power = glm::dot(balls[j]->velocity - balls[i]->velocity, jNorm);
				glm::vec2 impulse = power * jNorm;
				balls[i]->velocity += impulse;
				balls[j]->velocity -= impulse;

				// position correction
				float penetration = 2.0f * BALL_RADIUS - sqrt(deltaSq);
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

bool PoolTable::canPlaceWhiteBall() const
{
	glm::vec2 pos = balls[0]->position;
	if (std::abs(pos.x) > TABLE_WIDTH / 2 - BALL_RADIUS - CORNER_REDUCE)
		return false;
	if (std::abs(pos.y) > TABLE_HEIGHT / 2 - BALL_RADIUS - CORNER_REDUCE)
		return false;
	for (int i = 1; i < balls.size(); i++)
	{
		if (glm::length2(balls[i]->position - pos) < BALL_RADIUS * BALL_RADIUS * 4)
			return false;
	}
	return true;
}

RaytraceResult PoolTable::getRaytracedBall(glm::vec2 pos, glm::vec2 dir) const
{
	RaytraceResult result = { false, };
	float minDist = 12345678900;
	glm::vec2 N = glm::normalize(dir);
	glm::vec3 right = glm::cross(glm::vec3(N.x, 0, N.y), glm::vec3(0, 1, 0));

	for (unsigned int i = 1; i < balls.size(); i++)
	{
		glm::vec2 diff = balls[i]->position - pos;
		if (glm::dot(diff, dir) < 0)
			continue;

		float dist = std::abs(glm::dot(diff, glm::vec2(right.x, right.z)));
		if (dist < 2 * BALL_RADIUS)
		{
			float L = sqrt(glm::length2(diff) - dist * dist);
			float S = sqrt(BALL_RADIUS * BALL_RADIUS * 4 - dist * dist);

			glm::vec2 tHit = pos + N * (L - S);
			float sqlen = glm::length2(tHit - pos);

			if (sqlen < minDist)
			{
				result.hit = true;
				result.hitTimeBallPos = tHit;
				result.hitTargetBall = balls[i];
				minDist = sqlen;
			}
		}
	}

	return result;
}

Ball* PoolTable::addBall(float x, float y)
{
	Ball* ball = new Ball();
	ball->position.x = x;
	ball->position.y = y;
	ball->velocity = { 0, 0 };
	balls.push_back(ball);
	return ball;
}