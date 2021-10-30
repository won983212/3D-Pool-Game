#pragma once
#include <string>
#include "gfx.h"

namespace commoncg
{
	class VAO
	{
	public:
		VAO() : handle_(0) { }
		~VAO();
		void Create();
		void Destroy() const;
		void Use() const;
		void Attrib(GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) const;
		static void Unbind();
		void CheckIfCreated(const std::string& method_name) const;
		bool IsCreated() const;

	private:
		GLuint handle_;
	};
}
