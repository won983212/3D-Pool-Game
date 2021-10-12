#pragma once
#include "../gfx/vao.h"
#include "../gfx/vbo.h"

namespace model
{
	class Ball
	{
	public:
		void init(float radius, int step = 30);
		void draw();
	private:
		commoncg::VAO vao;
		commoncg::VBO vbo;
		int verticesSize;
	};
}