#pragma once
#include "../gfx/vao.h"
#include "../gfx/vbo.h"
#include "../gfx/texture.h"
#include "../gfx/shader.h"

#define BALL_RENDER_STEP 30
#define BALL_RADIUS 1.0f

namespace model
{
	class Ball
	{
	public:
		void init(const char* texturePath);
		void draw(commoncg::ShaderProgram& shader);
	private:
		commoncg::VAO vao;
		commoncg::VBO vbo;
		const commoncg::Texture* texture;
		int verticesSize;
	};
}