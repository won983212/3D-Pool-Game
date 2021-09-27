#include "vao.h"
#include "../util/util.h"

void VAO::create()
{
	glGenVertexArrays(1, &handle);
}

void VAO::destroy()
{
	glDeleteVertexArrays(1, &handle);
}

void VAO::bind() const
{
	glBindVertexArray(handle);
}

void VAO::attr(VBO vbo, GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) const
{
	bind();
	vbo.bind();
	glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void*)offset);
	glEnableVertexAttribArray(index);
}

void VAO::attr(GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) const
{
	bind();
	glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void*)offset);
	glEnableVertexAttribArray(index);
}

void VAO::unbind()
{
	glBindVertexArray(0);
}