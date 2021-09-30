#include <iostream>
#include "vao.h"

using namespace commoncg;

VAO::~VAO()
{
	destroy();
}

void VAO::create()
{
	glGenVertexArrays(1, &handle);
}

void VAO::destroy()
{
	checkIfCreated("destroy");
	glDeleteVertexArrays(1, &handle);
}

void VAO::bind() const
{
	checkIfCreated("bind");
	glBindVertexArray(handle);
}

void VAO::attr(VBO vbo, GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) const
{
	checkIfCreated("attr");
	bind();
	vbo.bind();
	glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void*)offset);
	glEnableVertexAttribArray(index);
}

void VAO::attr(GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) const
{
	checkIfCreated("attr");
	bind();
	glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void*)offset);
	glEnableVertexAttribArray(index);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}

void VAO::checkIfCreated(std::string methodName) const
{
	if (handle == -1)
	{
		std::string message = "Error: Called " + methodName + "() before creating.";
		std::cout << message << std::endl;
		throw message;
	}
}