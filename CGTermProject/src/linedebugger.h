#pragma once
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
	~LineDebugger();

public:
	void Init(int lines = 1);
	void Clear();
	void Add(float x, float y, float z);
	void Add(float x, float y, float z, int color);
	void Update() const;
	void Draw() const;

public:
	unsigned int color_ = 0xffff0000;

private:
	int len_ = 0;
	linedebugger::Vertex* points_ = nullptr;
	int point_size_ = 0;
	commoncg::ShaderProgram beam_shader_;
	commoncg::VAO beam_vao_;
	commoncg::VBO beam_vbo_;
};
