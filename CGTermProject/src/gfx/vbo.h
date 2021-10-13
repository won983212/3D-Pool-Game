#pragma once
#include "gfx.h"
#include <string>

namespace commoncg
{
	class VBO
	{
	public:
		VBO(int bufferType = GL_ARRAY_BUFFER);
		~VBO();
		void create();
		bool isCreated() const;
		void destroy() const;
		void use() const;
		void buffer(GLsizeiptr size, const void* data, GLenum drawingHint = GL_STATIC_DRAW) const;
		void subBuffer(GLintptr offset, GLsizeiptr size, const void* data) const;
		void bindBufferRange(GLuint index, GLintptr offset, GLsizeiptr size) const;
		void unbind() const;
	private:
		void checkIfCreated(std::string methodName) const;
	private:
		GLuint handle;
		GLenum bufferType;
	};
}