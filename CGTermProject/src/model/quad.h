#pragma once
#include "../gfx/gfx.h"
#include "../gfx/vao.h"
#include "../gfx/vbo.h"

namespace model
{
	class Quad
	{
	public:
		void Init(float width, float height, glm::vec3 normal);
		void Draw() const;

	private:
		commoncg::VAO vao_;
		commoncg::VBO vbo_;
		int vertices_size_ = 0;
	};
}
