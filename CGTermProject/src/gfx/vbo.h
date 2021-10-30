#pragma once
#include "gfx.h"
#include <string>

namespace commoncg
{
	class VBO
	{
	public:
		VBO(int buffer_type = GL_ARRAY_BUFFER);
		~VBO();
		void Create();
		bool IsCreated() const;
		void Destroy() const;
		void Use() const;
		void Buffer(GLsizeiptr size, const void* data, GLenum drawing_hint = GL_STATIC_DRAW) const;
		void SubBuffer(GLintptr offset, GLsizeiptr size, const void* data) const;
		void BindBufferRange(GLuint index, GLintptr offset, GLsizeiptr size) const;
		void Unbind() const;

	private:
		void CheckIfCreated(const std::string& method_name) const;

	private:
		GLuint handle_;
		GLenum buffer_type_;
	};
}
