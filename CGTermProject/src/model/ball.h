#pragma once
#include "../gfx/vao.h"
#include "../gfx/vbo.h"

namespace model
{
	class Ball
	{
	public:
		void Init(float radius, int step = 30);
		void Draw() const;

	private:
		commoncg::VAO vao_;
		commoncg::VBO vbo_;
		int vertices_size_ = 0;
	};
}
