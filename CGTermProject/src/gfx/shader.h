#pragma once

#include "gfx.h"

#define SHADER_MAX_COUNT 5

namespace commoncg
{
	struct Shader
	{
		const GLchar* path;
		GLenum type;
		GLuint id;
	};

	class ShaderProgram
	{
	public:
		~ShaderProgram();
		void addShader(const GLchar* path, GLenum type);
		void load();
		void use() const;
		void destroy() const;
		GLuint getProgramID() const;

		// Setting vectors
		void setUniform(const char* name, glm::vec2* vectors, int count = 1) const;
		void setUniform(const char* name, const glm::vec2 vector) const;
		void setUniform(const char* name, glm::vec3* vectors, int count = 1) const;
		void setUniform(const char* name, const glm::vec3 vector) const;
		void setUniform(const char* name, glm::vec4* vectors, int count = 1) const;
		void setUniform(const char* name, const glm::vec4& vector) const;

		// Setting floats
		void setUniform(const char* name, float* values, int count = 1) const;
		void setUniform(const char* name, const float value) const;

		// Setting 3x3 matrices
		void setUniform(const char* name, glm::mat3* matrices, int count = 1) const;
		void setUniform(const char* name, const glm::mat3 matrix) const;

		// Setting 4x4 matrices
		void setUniform(const char* name, glm::mat4* matrices, int count = 1) const;
		void setUniform(const char* name, const glm::mat4& matrix) const;

		// Setting integers
		void setUniform(const char* name, int* values, int count = 1) const;
		void setUniform(const char* name, const int value) const;

	private:
		GLuint handle;
		Shader shaders[SHADER_MAX_COUNT];
		int shader_index = 0;
	};
}