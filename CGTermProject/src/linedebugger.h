#pragma once
#include <vector>
#include "gfx/gfx.h"
#include "gfx/vao.h"
#include "gfx/vbo.h"
#include "gfx/shader.h"

class LineDebugger
{
public:
	~LineDebugger()
	{
		delete[] points;
	}

public:
	void init(int lines = 1);
	void update();
	void setColor(int color);
	void draw();

public:
	int color = 0xff0000;
	glm::vec3* points;

private:
	int pointSize;
	commoncg::ShaderProgram beamShader;
	commoncg::VAO beamVao;
	commoncg::VBO beamVbo;
};