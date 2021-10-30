#include <iostream>
#include "linedebugger.h"
#include "util/util.h"

using namespace commoncg;

LineDebugger::~LineDebugger()
{
	delete[] points_;
}

void LineDebugger::Init(int lines)
{
	points_ = new linedebugger::Vertex[lines * 2];
	point_size_ = lines * 2;
	memset(points_, 0, sizeof(linedebugger::Vertex) * point_size_);

	beam_vao_.Create();
	beam_vbo_.Create();
	beam_vao_.Use();
	beam_vbo_.Buffer(SIZEOF(linedebugger::Vertex, point_size_), points_, GL_DYNAMIC_DRAW);
	beam_vao_.Attrib(0, 3, GL_FLOAT, SIZEOF(float, 7), 0);
	beam_vao_.Attrib(1, 4, GL_FLOAT, SIZEOF(float, 7), SIZEOF(float, 3));
	VAO::Unbind();

	ShaderProgram::Push();
	beam_shader_.AddShader("res/shader/ray.vert", GL_VERTEX_SHADER);
	beam_shader_.AddShader("res/shader/ray.frag", GL_FRAGMENT_SHADER);
	beam_shader_.Load();
	beam_shader_.Use();
	beam_shader_.SetUniform("model", glm::mat4(1.0f));
	ShaderProgram::Pop();
}

void LineDebugger::Clear()
{
	len_ = 0;
}

void LineDebugger::Add(float x, float y, float z)
{
	Add(x, y, z, color_);
}

void LineDebugger::Add(float x, float y, float z, int color)
{
	if (len_ >= point_size_)
	{
		std::cout << "Error: Full size points" << std::endl;
		return;
	}

	float a = (color >> 24 & 0xff) / 255.0f;
	float r = (color >> 16 & 0xff) / 255.0f;
	float g = (color >> 8 & 0xff) / 255.0f;
	float b = (color & 0xff) / 255.0f;

	linedebugger::Vertex v = {{x, y, z}, {r, g, b, a}};
	points_[len_++] = v;
}

void LineDebugger::Update() const
{
	beam_vao_.Use();
	beam_vbo_.Buffer(SIZEOF(linedebugger::Vertex, point_size_), points_, GL_DYNAMIC_DRAW);
	beam_vbo_.Unbind();
	VAO::Unbind();
}

void LineDebugger::Draw() const
{
	ShaderProgram::Push();
	beam_shader_.Use();
	beam_vao_.Use();
	glDrawArrays(GL_LINES, 0, point_size_);
	VAO::Unbind();
	ShaderProgram::Pop();
}
