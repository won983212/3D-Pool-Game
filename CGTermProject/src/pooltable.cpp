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


constexpr float Sq3 = 1.73205080756f;
constexpr float InvSq2 = 0.70710678118f;
constexpr float CornerReduce = TableHoleSize * InvSq2 + TableWallThickness;
constexpr float SideCornerReduce = CornerReduce - TableHoleSize * 0.5f;

// line walls.
constexpr StaticWall Walls[] =
{
	{{0, TableHeight / 2 - TableWallThickness}, {0, -1}, TableWidth - 2 * CornerReduce},
	{{0, -TableHeight / 2 + TableWallThickness}, {0, 1}, TableWidth - 2 * CornerReduce},
	{{-TableWidth / 2 + TableWallThickness, TableHeight / 4 - SideCornerReduce / 2}, {1, 0}, TableHeight / 2 - TableHoleSize - SideCornerReduce},
	{{-TableWidth / 2 + TableWallThickness, -TableHeight / 4 + SideCornerReduce / 2}, {1, 0}, TableHeight / 2 - TableHoleSize - SideCornerReduce},
	{{TableWidth / 2 - TableWallThickness, TableHeight / 4 - SideCornerReduce / 2}, {-1, 0}, TableHeight / 2 - TableHoleSize - SideCornerReduce},
	{{TableWidth / 2 - TableWallThickness, -TableHeight / 4 + SideCornerReduce / 2}, {-1, 0}, TableHeight / 2 - TableHoleSize - SideCornerReduce}
};

// holes.
constexpr glm::vec2 Holes[] =
{
	{-TableWidth / 2, 0.0f},
	{TableWidth / 2, 0.0f},
	{TableWidth / 2 - TableWallThickness, TableHeight / 2 - TableWallThickness},
	{TableWidth / 2 - TableWallThickness, -TableHeight / 2 + TableWallThickness},
	{-TableWidth / 2 + TableWallThickness, TableHeight / 2 - TableWallThickness},
	{-TableWidth / 2 + TableWallThickness, -TableHeight / 2 + TableWallThickness}
};

constexpr float FrictionModifier = 0.99f;
constexpr int BallIndexes[] = {1, 11, 3, 6, 8, 14, 13, 15, 4, 9, 7, 2, 10, 5, 12};


PoolTable::PoolTable()
{
	constexpr float gap = TableHeight * 0.8f / 15.0f;
	for (int i = 0; i < BallCount; i++)
		AddBall(0.0f, i * gap - TableHeight * 0.8f / 2.0f);
}

PoolTable::~PoolTable()
{
	for (const auto& ball : balls_)
		delete ball;
}

void PoolTable::ResetBallPosition()
{
	stop_event_called_ = true;

	// white(0) ball setup
	balls_[0]->position_ = glm::vec2(0.0f, -TableHeight / 3);
	balls_[0]->velocity_ = glm::vec2(0.0f);
	balls_[0]->visible_ = true;

	// 1~15 ball setup
	int i = 0;
	for (int row = 0; row < 5; row++)
	{
		for (int column = 0; column <= row; column++)
		{
			glm::vec2 location = glm::vec2(2 * column - row, row * Sq3) * BallRadius;
			location.y += TableHeight / 4;
			balls_[BallIndexes[i]]->position_ = location;
			balls_[BallIndexes[i]]->velocity_ = glm::vec2(0.0f);
			balls_[BallIndexes[i++]]->visible_ = true;
		}
	}
}

void PoolTable::Update(float partial_time)
{
	bool is_all_stop = true;
	for (unsigned int i = 0; i < balls_.size(); i++)
	{
		Ball* ball = balls_[i];
		if (!ball->visible_)
			continue;

		// update velocity
		float vel = length(ball->velocity_);
		if (vel > 0)
			ball->velocity_ *= FrictionModifier; // friction

		// velocity가 너무 낮으면 정지
		if (vel * FrictionModifier < 0.02)
		{
			ball->velocity_ = glm::vec2(0.0f);
			vel = 0;
		}
		else
		{
			is_all_stop = false;
		}

		// update position
		ball->position_ += partial_time * ball->velocity_;

		// update rotation
		if (vel > 0)
		{
			auto vel3d = glm::vec3(ball->velocity_.x, 0, ball->velocity_.y);
			glm::vec3 rotation_axis = cross(glm::vec3(0, 1, 0), vel3d);
			float d_angle = partial_time * length(ball->velocity_) / BallRadius; // dt*|v|/r
			ball->rotation_ = angleAxis(d_angle, normalize(rotation_axis)) * ball->rotation_;
		}

		constexpr int hole_size = sizeof(Holes) / sizeof(glm::vec2);
		constexpr float adj_hole_size = (TableHoleSize + 0.2f) * 0.5f;

		// in hole test
		unsigned int j;
		for (j = 0; j < hole_size; j++)
		{
			if (length2(Holes[j] - ball->position_) < adj_hole_size * adj_hole_size)
				break;
		}

		// if ball is in hole, hide it.
		if (j < hole_size)
		{
			ball->visible_ = false;
			ball->velocity_ = glm::vec2(0.0f);
			if (ball_event_ != nullptr)
				ball_event_->OnBallHoleIn(i);
			continue;
		}

		// collision check & resolving. 1회만 test하기 위해 범위는 0~i로 설정.
		// add velocity and I after solving all collision.
		glm::vec2 I(0.0f);
		glm::vec2 v = balls_[i]->velocity_;
		glm::vec2 v_norm = normalize(v);
		float len_v = length(v);
		for (j = 0; j < balls_.size(); j++)
		{
			glm::vec2 delta = balls_[j]->position_ - balls_[i]->position_;
			if (i == j || !balls_[j]->visible_ || dot(v, delta) < 0) // 정방향으로 충돌한 ball만 고려
				continue;

			float delta_sq = length2(delta);
			float delta_dot = dot(delta, v_norm);
			float dist = delta_sq - delta_dot * delta_dot;

			// collision test. ray test and radius test
			bool collide = delta_sq + 0.0001f < 4 * BallRadius * BallRadius || dist < 4 * BallRadius * BallRadius && delta_dot < len_v * partial_time;
			if (!collide)
				continue;

			// discrete position correction (discrete하게 position을 update하므로 정확한 impulse적용을 위해 보정이 필요함)
			if (length2(v) > 0)
			{
				glm::vec2 extend_pos = balls_[i]->position_ - 10.0f * v_norm;
				glm::vec2 d_pos = balls_[j]->position_ - extend_pos;
				float ph = dot(v_norm, d_pos);
				float lh = 4 * BallRadius * BallRadius - length2(d_pos) + ph * ph;

				// float error problem. 가끔 float error문제로 음수가 나오는데, ignore
				if (lh >= 0)
				{
					balls_[i]->position_ = extend_pos + (ph - sqrt(lh)) * v_norm;
					delta = balls_[j]->position_ - balls_[i]->position_;
				}
			}

			// apply impulse
			glm::vec2 j_norm = normalize(delta);
			float power = dot(balls_[j]->velocity_ - v, j_norm);
			glm::vec2 impulse = power * j_norm;
			I += impulse;
			balls_[j]->velocity_ -= impulse;

			// position correction
			float penetration = 2.0f * BallRadius - sqrt(delta_sq);
			balls_[i]->position_ += penetration / 2 * -j_norm;
			balls_[j]->position_ += penetration / 2 * j_norm;

			// play sound
			power = std::abs(power);
			int power_level = static_cast<int>(power / 2.0f);
			irrklang::ISound* sound = GetSoundEngine()->play2D(SOUND_BALL_COLLIDE(std::min(power_level, 2)), false, false, true);
			sound->setVolume(glm::clamp(power - power_level * 2.0f, 0.3f, 1.0f));
			sound->drop();

			if (i == 0 && ball_event_ != nullptr)
				ball_event_->OnWhiteBallCollide(j);
		}
		balls_[i]->velocity_ += I;

		// wall collision test
		for (const auto& wall : Walls)
		{
			glm::vec2 diff = ball->position_ - wall.position;
			glm::vec2 wall_norm = normalize(wall.normal);
			float dist = dot(diff, wall_norm);
			if (dist < BallRadius && (length2(diff) - dist * dist) * 4 < wall.size * wall.size)
			{
				ball->position_ += (BallRadius - dist) * wall_norm;
				ball->velocity_ = reflect(ball->velocity_, wall_norm);
			}
		}
	}

	if (is_all_stop)
	{
		if (!stop_event_called_)
		{
			if (ball_event_ != nullptr)
				ball_event_->OnAllBallStopped();
			stop_event_called_ = true;
		}
	}
	else
	{
		stop_event_called_ = false;
	}
}

bool PoolTable::CanPlaceWhiteBall() const
{
	const glm::vec2 pos = balls_[0]->position_;
	if (std::abs(pos.x) > TableWidth / 2 - BallRadius - CornerReduce)
		return false;
	if (std::abs(pos.y) > TableHeight / 2 - BallRadius - CornerReduce)
		return false;
	for (int i = 1; i < balls_.size(); i++)
	{
		if (length2(balls_[i]->position_ - pos) < BallRadius * BallRadius * 4)
			return false;
	}
	return true;
}

RaytraceResult PoolTable::GetRaytracedBall(glm::vec2 pos, glm::vec2 dir) const
{
	RaytraceResult result = {false,};
	float min_dist = 1234567890.0f;
	glm::vec2 N = normalize(dir);
	glm::vec3 right = cross(glm::vec3(N.x, 0, N.y), glm::vec3(0, 1, 0));

	// ball collision check
	for (unsigned int i = 1; i < balls_.size(); i++)
	{
		glm::vec2 diff = balls_[i]->position_ - pos;
		if (dot(diff, dir) < 0 || !balls_[i]->visible_)
			continue;

		float dist = std::abs(dot(diff, glm::vec2(right.x, right.z)));
		if (dist < 2 * BallRadius)
		{
			float L = sqrt(length2(diff) - dist * dist);
			float S = sqrt(BallRadius * BallRadius * 4 - dist * dist);

			glm::vec2 t_hit = pos + N * (L - S);
			float sqlen = length2(t_hit - pos);

			if (sqlen < min_dist)
			{
				result.hit = true;
				result.hit_ball_pos = t_hit;
				result.hit_target_ball = balls_[i];
				min_dist = sqlen;
			}
		}
	}

	// if result has ball collision, return
	if (result.hit)
		return result;

	// wall collision check
	for (const auto& wall : Walls)
	{
		glm::vec2 norm = normalize(wall.normal);
		float t = -dot(pos - wall.position, norm) / dot(dir, norm);
		glm::vec2 hit_pos = pos + t * dir;
		if (t > 0 && length(hit_pos - wall.position) < wall.size / 2 + BallRadius)
		{
			result.hit = true;
			result.hit_target_ball = nullptr;
			result.hit_ball_pos = hit_pos - BallRadius * dir;
			result.normal = wall.normal;
			return result;
		}
	}

	return result;
}

Ball* PoolTable::AddBall(float x, float y)
{
	const auto ball = new Ball();
	ball->position_.x = x;
	ball->position_.y = y;
	ball->velocity_ = {0, 0};
	balls_.push_back(ball);
	return ball;
}
