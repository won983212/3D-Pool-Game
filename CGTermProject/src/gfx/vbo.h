#pragma once
#include "gfx.h"

namespace commoncg
{
	class VBO
	{
	public:
		VBO(int bufferType = GL_ARRAY_BUFFER);
		void create();
		void destroy() const;
		void bind() const;
		void buffer(GLsizeiptr size, const void* data, GLenum drawingHint = GL_STATIC_DRAW) const;
		void bindBufferRange(GLuint index, GLintptr offset, GLsizeiptr size) const;
		void unbind() const;
	private:
		GLuint handle;
		GLenum bufferType;
	};
}