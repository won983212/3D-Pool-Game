#include <iostream>
#include "vao.h"

using namespace commoncg;

VAO::~VAO()
{
	Destroy();
}

void VAO::Create()
{
	glGenVertexArrays(1, &handle_);
}

void VAO::Destroy() const
{
	CheckIfCreated("Destroy");
	glDeleteVertexArrays(1, &handle_);
}

void VAO::Use() const
{
	CheckIfCreated("Use");
	glBindVertexArray(handle_);
}

void VAO::Attrib(GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) const
{
	CheckIfCreated("Attrib");
	Use();
	glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void*)offset);
	glEnableVertexAttribArray(index);
}

void VAO::Unbind()
{
	glBindVertexArray(0);
}

void VAO::CheckIfCreated(const std::string& method_name) const
{
	if (handle_ == 0)
	{
		std::string message = "Error: Called " + method_name + "() before creating.";
		std::cout << message << std::endl;
		throw message;
	}
}

bool VAO::IsCreated() const
{
	return handle_ != 0;
}
