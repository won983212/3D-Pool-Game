#pragma once
#include "gfx.h"
#include "vbo.h"

namespace commoncg
{
	class VAO
	{
	public:
		void create();
		void destroy();
		void bind() const;
		void attr(VBO vbo, GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) const;
		void attr(GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) const;
		static void unbind();
	private:
		GLuint handle;
	};
}