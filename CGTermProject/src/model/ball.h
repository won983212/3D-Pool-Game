#pragma once
#include "../gfx/vao.h"
#include "../gfx/vbo.h"
#include "../gfx/texture.h"
#include "../gfx/shader.h"

namespace model
{
	class Ball
	{
	public:
		void init();
		void draw(commoncg::ShaderProgram& shader);
	private:
		commoncg::VAO vao;
		commoncg::VBO vbo;
		int verticesSize;
	};
}