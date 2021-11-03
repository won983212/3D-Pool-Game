#pragma once
#include "gfx/vao.h"
#include "gfx/vbo.h"
#include "gfx/shader.h"
#include "pooltable.h"

class BallTracer
{
public:
	BallTracer(const PoolTable& table)
		: position_(0.0f), direction_(0.0f), table_(table)
	{
	}

	void Init();
	void Update();
	void Draw() const;

public:
	glm::vec2 position_;
	glm::vec2 direction_;

private:
	const PoolTable& table_;
	commoncg::ShaderProgram beam_shader_;
	commoncg::VAO beam_vao_;
	commoncg::VBO beam_vbo_;
	commoncg::VAO circle_vao_;
	commoncg::VBO circle_vbo_;
	glm::vec3 hit_ball_pos_{};
	bool visible_ = false;
};
