#pragma once

#include "gfx.h"

constexpr int MaxShaderCount = 5;

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
		void AddShader(const GLchar* path, GLenum type);
		void Load();
		void Use() const;
		void Destroy() const;
		GLuint GetProgramId() const;

		// Setting vectors
		void SetUniform(const char* name, glm::vec2* vectors, int count = 1) const;
		void SetUniform(const char* name, glm::vec2 vector) const;
		void SetUniform(const char* name, glm::vec3* vectors, int count = 1) const;
		void SetUniform(const char* name, glm::vec3 vector) const;
		void SetUniform(const char* name, glm::vec4* vectors, int count = 1) const;
		void SetUniform(const char* name, const glm::vec4& vector) const;

		// Setting floats
		void SetUniform(const char* name, const float* values, int count = 1) const;
		void SetUniform(const char* name, float value) const;

		// Setting 3x3 matrices
		void SetUniform(const char* name, glm::mat3* matrices, int count = 1) const;
		void SetUniform(const char* name, glm::mat3 matrix) const;

		// Setting 4x4 matrices
		void SetUniform(const char* name, glm::mat4* matrices, int count = 1) const;
		void SetUniform(const char* name, const glm::mat4& matrix) const;

		// Setting integers
		void SetUniform(const char* name, const int* values, int count = 1) const;
		void SetUniform(const char* name, int value) const;

		static void Push();
		static void Pop();
		static const ShaderProgram* GetContextShader();

	private:
		GLuint handle_ = 0;
		Shader shaders_[MaxShaderCount] = {};
		int shader_index_ = 0;
	};
}
