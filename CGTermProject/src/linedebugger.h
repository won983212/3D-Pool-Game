#pragma once
#include <vector>
#include "gfx/gfx.h"
#include "gfx/vao.h"
#include "gfx/vbo.h"
#include "gfx/shader.h"

namespace linedebugger
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
	};
}

class LineDebugger
{
public:
	~LineDebugger()
	{
		delete[] points;
	}

public:
	void init(int lines = 1);
	void clear();
	void add(float x, float y, float z);
	void add(float x, float y, float z, int color);
	void update();
	void draw();

public:
	int color = 0xffff0000;

private:
	int len = 0;
	linedebugger::Vertex* points;
	int pointSize;
	commoncg::ShaderProgram beamShader;
	commoncg::VAO beamVao;
	commoncg::VBO beamVbo;
};