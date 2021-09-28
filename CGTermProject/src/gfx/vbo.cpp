#include "vbo.h"
#include "../util/util.h"

VBO::VBO(int bufferType)
	:bufferType(bufferType)
{ }

void VBO::create()
{
	glGenBuffers(1, &handle);
}

void VBO::destroy() const
{
	glDeleteBuffers(1, &handle);
}

void VBO::bind() const
{
	glBindBuffer(bufferType, handle);
}

void VBO::buffer(GLsizeiptr size, const void* data, GLenum drawingHint) const
{
	bind();
	glBufferData(bufferType, size, data, drawingHint);
}

void VBO::bindBufferRange(GLuint index, GLintptr offset, GLsizeiptr size) const
{
	glBindBufferRange(bufferType, index, handle, offset, size);
}

void VBO::unbind() const
{
	glBindBuffer(bufferType, 0);
}