#pragma once

#include "gfx.h"
#include "shader.h"
#include "vao.h"
#include "vbo.h"

namespace commoncg
{
	class Skybox
	{
	public:
		void BeginLoad();
		void LoadHdrSkybox(const char* hdr_texture_path);
		void LoadDdsIrradianceMap(const char* dds_texture_path);
		void LoadDdsSpecularMap(const char* dds_texture_path);
		void EndLoad() const;
		void Render(glm::mat4 view) const;
		void BindEnvironmentTextures() const;

	private:
		GLuint LoadEquirectangularMap(const char* texture_path, int width, int height, bool flip_y) const;

	private:
		ShaderProgram skybox_shader_;
		GLuint texture_id_ = 0, irr_texture_id_ = 0, specular_texture_id_ = 0;
		VAO vao_;
		VBO vbo_;
		GLuint fbo_ = 0, rbo_ = 0;
		ShaderProgram cubemap_convert_shader_;
	};
}
