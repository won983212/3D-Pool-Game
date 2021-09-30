#include <iostream>
#include "vbo.h"

using namespace commoncg;

VBO::VBO(int bufferType)
	:bufferType(bufferType), handle(-1)
{ }

VBO::~VBO()
{
	destroy();
}

void VBO::create()
{
	glGenBuffers(1, &handle);
}

bool VBO::isCreated() const
{
	return handle != -1;
}

void VBO::destroy() const
{
	checkIfCreated("destroy");
	glDeleteBuffers(1, &handle);
}

void VBO::bind() const
{
	checkIfCreated("bind");
	glBindBuffer(bufferType, handle);
}

void VBO::buffer(GLsizeiptr size, const void* data, GLenum drawingHint) const
{
	checkIfCreated("buffer");
	bind();
	glBufferData(bufferType, size, data, drawingHint);
}

void VBO::bindBufferRange(GLuint index, GLintptr offset, GLsizeiptr size) const
{
	checkIfCreated("bindBufferRange");
	glBindBufferRange(bufferType, index, handle, offset, size);
}

void VBO::unbind() const
{
	glBindBuffer(bufferType, 0);
}

void VBO::checkIfCreated(std::string methodName) const
{
	if (!isCreated())
	{
		std::string message = "Error: Called " + methodName + "() before creating.";
		std::cout << message << std::endl;
		throw message;
	}
}