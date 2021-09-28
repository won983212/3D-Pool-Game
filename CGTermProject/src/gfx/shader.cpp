#include <iostream>
#include <fstream>
#include <sstream>
#include "shader.h"
#include "../util/util.h"


static GLuint _compileShaderSource(const char* filePath, GLenum shaderType)
{
	// read file (cpp stream style)
	std::string code;
	std::ifstream fileStream;

	fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		std::stringstream strStream;
		fileStream.open(filePath);
		strStream << fileStream.rdbuf();
		fileStream.close();
		code = strStream.str();
	}
	catch (std::ifstream::failure e)
	{
		std::cout << "Error: Can't read file: " << filePath << std::endl;
		throw e;
	}


	// compile shader
	const char* data = code.c_str();
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &data, NULL);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Error: compile shader: (" << filePath << ")" << infoLog << std::endl;
		throw "Error: compile shader";
	}

	return shader;
}

ShaderProgram::~ShaderProgram()
{
	destroy();
}

void ShaderProgram::addShader(const GLchar* path, GLenum type)
{
	if (shader_index >= SHADER_MAX_COUNT)
	{
		std::cout << "Error: shader is full: " << path << std::endl;
		return;
	}

	shaders[shader_index++] = { path, type };
}

void ShaderProgram::load()
{
	int success;
	char infoLog[512];

	handle = glCreateProgram();
	for (int i = 0; i < shader_index; i++)
	{
		GLuint shader = _compileShaderSource(shaders[i].path, shaders[i].type);
		shaders[i].id = shader;
		glAttachShader(handle, shader);
	}
	glLinkProgram(handle);

	glGetShaderiv(handle, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(handle, 512, NULL, infoLog);
		std::cout << "Error: link shader program: " << infoLog << std::endl;
		throw "Error: link shader program";
	}

	for (int i = 0; i < shader_index; i++)
		glDeleteShader(shaders[i].id);
	shader_index = 0;
}

void ShaderProgram::use() const
{
	glUseProgram(handle);
}


void ShaderProgram::destroy() const
{
	glDeleteProgram(handle);
}

GLuint ShaderProgram::getProgramID() const
{
	return handle;
}

// Setting floats
void ShaderProgram::setUniform(const char* name, float* values, int count) const
{
	glUniform1fv(glGetUniformLocation(handle, name), count, values);
}

void ShaderProgram::setUniform(const char* name, const float fValue) const
{
	glUniform1fv(glGetUniformLocation(handle, name), 1, &fValue);
}

// Setting vectors
void ShaderProgram::setUniform(const char* name, glm::vec2* vectors, int count) const
{
	glUniform2fv(glGetUniformLocation(handle, name), count, (GLfloat*)vectors);
}

void ShaderProgram::setUniform(const char* name, const glm::vec2 vector) const
{
	glUniform2fv(glGetUniformLocation(handle, name), 1, (GLfloat*)&vector);
}

void ShaderProgram::setUniform(const char* name, glm::vec3* vectors, int count) const
{
	glUniform3fv(glGetUniformLocation(handle, name), count, (GLfloat*)vectors);
}

void ShaderProgram::setUniform(const char* name, const glm::vec3 vector) const
{
	glUniform3fv(glGetUniformLocation(handle, name), 1, (GLfloat*)&vector);
}

void ShaderProgram::setUniform(const char* name, glm::vec4* vectors, int count) const
{
	glUniform4fv(glGetUniformLocation(handle, name), count, (GLfloat*)vectors);
}

void ShaderProgram::setUniform(const char* name, const glm::vec4& vector) const
{
	glUniform4fv(glGetUniformLocation(handle, name), 1, (GLfloat*)&vector);
}

// Setting 3x3 matrices
void ShaderProgram::setUniform(const char* name, glm::mat3* matrices, int count) const
{
	glUniformMatrix3fv(glGetUniformLocation(handle, name), count, FALSE, (GLfloat*)matrices);
}

void ShaderProgram::setUniform(const char* name, const glm::mat3 matrix) const
{
	glUniformMatrix3fv(glGetUniformLocation(handle, name), 1, FALSE, (GLfloat*)&matrix);
}

// Setting 4x4 matrices
void ShaderProgram::setUniform(const char* name, glm::mat4* matrices, int count) const
{
	glUniformMatrix4fv(glGetUniformLocation(handle, name), count, FALSE, (GLfloat*)matrices);
}

void ShaderProgram::setUniform(const char* name, const glm::mat4& matrix) const
{
	glUniformMatrix4fv(glGetUniformLocation(handle, name), 1, FALSE, (GLfloat*)&matrix);
}

// Setting integers
void ShaderProgram::setUniform(const char* name, int* values, int count) const
{
	glUniform1iv(glGetUniformLocation(handle, name), count, values);
}

void ShaderProgram::setUniform(const char* name, const int iValue) const
{
	glUniform1i(glGetUniformLocation(handle, name), iValue);
}