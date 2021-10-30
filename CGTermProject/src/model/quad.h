#pragma once
#include "../gfx/gfx.h"
#include "../gfx/vao.h"
#include "../gfx/vbo.h"

namespace model
{
	class Quad
	{
	public:
		void init(float width, float height, glm::vec3 normal);
		void draw();

	private:
		commoncg::VAO vao_;
		commoncg::VBO vbo_;
		int vertices_size_ = 0;
	};
}
