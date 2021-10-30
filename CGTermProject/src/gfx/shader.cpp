#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>
#include "shader.h"

using namespace commoncg;

constexpr int MaxStackSize = 1000;

std::stack<const ShaderProgram*> shader_stack;
const ShaderProgram* last_uses = nullptr;

static GLuint CompileShaderSource(const char* file_path, const GLenum shader_type)
{
	// read file (cpp stream style)
	std::string code;
	std::ifstream file_stream;

	file_stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		std::stringstream str_stream;
		file_stream.open(file_path);
		str_stream << file_stream.rdbuf();
		file_stream.close();
		code = str_stream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Error: Can't read file: " << file_path << std::endl;
		throw;
	}


	// compile shader
	const char* data = code.c_str();
	const GLuint shader = glCreateShader(shader_type);
	glShaderSource(shader, 1, &data, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char info_log[512];
		glGetShaderInfoLog(shader, 512, nullptr, info_log);
		std::cout << "Error: compile shader: (" << file_path << ")" << info_log << std::endl;
		throw "Error: compile shader";
	}

	return shader;
}

ShaderProgram::~ShaderProgram()
{
	Destroy();
}

void ShaderProgram::AddShader(const GLchar* path, GLenum type)
{
	if (shader_index_ >= SHADER_MAX_COUNT)
	{
		std::cout << "Error: shader is full: " << path << std::endl;
		return;
	}

	shaders_[shader_index_++] = {path, type};
}

void ShaderProgram::Load()
{
	int success;

	handle_ = glCreateProgram();
	for (int i = 0; i < shader_index_; i++)
	{
		const GLuint shader = CompileShaderSource(shaders_[i].path, shaders_[i].type);
		shaders_[i].id = shader;
		glAttachShader(handle_, shader);
	}

	glLinkProgram(handle_);
	glGetShaderiv(handle_, GL_LINK_STATUS, &success);

	if (!success)
	{
		char info_log[512];
		glGetShaderInfoLog(handle_, 512, nullptr, info_log);
		std::cout << "Error: link shader program: " << info_log << std::endl;
		return;
	}

	for (int i = 0; i < shader_index_; i++)
		glDeleteShader(shaders_[i].id);
	shader_index_ = 0;
}

void ShaderProgram::Use() const
{
	last_uses = this;
	glUseProgram(handle_);
}

void ShaderProgram::Destroy() const
{
	glDeleteProgram(handle_);
}

GLuint ShaderProgram::GetProgramId() const
{
	return handle_;
}

// Setting floats
void ShaderProgram::SetUniform(const char* name, float* values, int count) const
{
	glUniform1fv(glGetUniformLocation(handle_, name), count, values);
}

void ShaderProgram::SetUniform(const char* name, const float f_value) const
{
	glUniform1fv(glGetUniformLocation(handle_, name), 1, &f_value);
}

// Setting vectors
void ShaderProgram::SetUniform(const char* name, glm::vec2* vectors, int count) const
{
	glUniform2fv(glGetUniformLocation(handle_, name), count, (GLfloat*)vectors);
}

void ShaderProgram::SetUniform(const char* name, const glm::vec2 vector) const
{
	glUniform2fv(glGetUniformLocation(handle_, name), 1, (GLfloat*)&vector);
}

void ShaderProgram::SetUniform(const char* name, glm::vec3* vectors, int count) const
{
	glUniform3fv(glGetUniformLocation(handle_, name), count, (GLfloat*)vectors);
}

void ShaderProgram::SetUniform(const char* name, const glm::vec3 vector) const
{
	glUniform3fv(glGetUniformLocation(handle_, name), 1, (GLfloat*)&vector);
}

void ShaderProgram::SetUniform(const char* name, glm::vec4* vectors, int count) const
{
	glUniform4fv(glGetUniformLocation(handle_, name), count, (GLfloat*)vectors);
}

void ShaderProgram::SetUniform(const char* name, const glm::vec4& vector) const
{
	glUniform4fv(glGetUniformLocation(handle_, name), 1, (GLfloat*)&vector);
}

// Setting 3x3 matrices
void ShaderProgram::SetUniform(const char* name, glm::mat3* matrices, int count) const
{
	glUniformMatrix3fv(glGetUniformLocation(handle_, name), count, FALSE, (GLfloat*)matrices);
}

void ShaderProgram::SetUniform(const char* name, const glm::mat3 matrix) const
{
	glUniformMatrix3fv(glGetUniformLocation(handle_, name), 1, FALSE, (GLfloat*)&matrix);
}

// Setting 4x4 matrices
void ShaderProgram::SetUniform(const char* name, glm::mat4* matrices, int count) const
{
	glUniformMatrix4fv(glGetUniformLocation(handle_, name), count, FALSE, (GLfloat*)matrices);
}

void ShaderProgram::SetUniform(const char* name, const glm::mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(handle_, name), 1, FALSE, (GLfloat*)&matrix);
}

// Setting integers
void ShaderProgram::SetUniform(const char* name, int* values, int count) const
{
	glUniform1iv(glGetUniformLocation(handle_, name), count, values);
}

void ShaderProgram::SetUniform(const char* name, const int value) const
{
	glUniform1i(glGetUniformLocation(handle_, name), value);
}

void ShaderProgram::Push()
{
	if (last_uses == nullptr)
		return;

	if (shader_stack.size() < MaxStackSize)
		shader_stack.push(last_uses);
	else
		std::cout << "Error: Shader stack overflow" << std::endl;
}

void ShaderProgram::Pop()
{
	if (shader_stack.empty())
		return;

	shader_stack.top()->Use();
	shader_stack.pop();
}

const ShaderProgram* ShaderProgram::GetContextShader()
{
	if (last_uses == nullptr)
		std::cout << "Warning: Current context has not shader." << std::endl;
	return last_uses;
}
