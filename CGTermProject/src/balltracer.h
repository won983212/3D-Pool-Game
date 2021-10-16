#pragma once
#include <glm/gtx/quaternion.hpp>
#include "gfx/vao.h"
#include "gfx/vbo.h"
#include "gfx/shader.h"
#include "pooltable.h"

class BallTracer
{
public:
	BallTracer(const PoolTable& table) 
		: table(table), position(0.0f), direction(0.0f) {};
	void init();
	void update();
	void draw();
public:
	glm::vec2 position;
	glm::vec2 direction;
private:
	const PoolTable& table;
	commoncg::ShaderProgram beamShader;
	commoncg::VAO beamVao;
	commoncg::VBO beamVbo;
	commoncg::VAO circleVao;
	commoncg::VBO circleVbo;
	glm::vec3 hitBallPos;
	bool visible = false;
};