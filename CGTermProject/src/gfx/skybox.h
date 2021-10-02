#pragma once

#include "gfx.h"
#include "shader.h"
#include "vao.h"
#include "vbo.h"

namespace commoncg
{
	class Skybox
	{
	public:
		void load(const char* textureDirPath, const char* skyboxVertShader, const char* skyboxFragShader);
		void render(const ShaderProgram& shader) const;
	private:
		ShaderProgram skyboxShader;
		GLuint textureId;
		VAO vao;
		VBO vbo;
	};
}