#include <iostream>
#include "vbo.h"

using namespace commoncg;

VBO::VBO(int buffer_type)
	: handle_(0), buffer_type_(buffer_type)
{
}

VBO::~VBO()
{
	Destroy();
}

void VBO::Create()
{
	glGenBuffers(1, &handle_);
}

bool VBO::IsCreated() const
{
	return handle_ != 0;
}

void VBO::Destroy() const
{
	CheckIfCreated("Destroy");
	glDeleteBuffers(1, &handle_);
}

void VBO::Use() const
{
	CheckIfCreated("Use");
	glBindBuffer(buffer_type_, handle_);
}

void VBO::Buffer(GLsizeiptr size, const void* data, GLenum drawing_hint) const
{
	CheckIfCreated("Buffer");
	Use();
	glBufferData(buffer_type_, size, data, drawing_hint);
}

void VBO::SubBuffer(GLintptr offset, GLsizeiptr size, const void* data) const
{
	CheckIfCreated("SubBuffer");
	Use();
	glBufferSubData(buffer_type_, offset, size, data);
}

void VBO::BindBufferRange(GLuint index, GLintptr offset, GLsizeiptr size) const
{
	CheckIfCreated("BindBufferRange");
	glBindBufferRange(buffer_type_, index, handle_, offset, size);
}

void VBO::Unbind() const
{
	glBindBuffer(buffer_type_, 0);
}

void VBO::CheckIfCreated(const std::string& method_name) const
{
	if (!IsCreated())
	{
		std::string message = "Error: Called " + method_name + "() before creating.";
		std::cout << message << std::endl;
		throw message;
	}
}
