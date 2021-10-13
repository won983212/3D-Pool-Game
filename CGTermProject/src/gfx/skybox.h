#pragma once

#include "gfx.h"
#include "shader.h"
#include "texture.h"
#include "vao.h"
#include "vbo.h"

namespace commoncg
{
	class Skybox
	{
	public:
		void beginLoad();
		void loadHDRSkybox(const char* hdrTexturePath);
		void loadDDSIrradianceMap(const char* ddsTexturePath);
		void loadDDSSpecularMap(const char* ddsTexturePath);
		void endLoad();
		void render(glm::mat4 view) const;
		void bindEnvironmentTextures() const;
	private:
		GLuint loadEquirectangularMap(const char* texturePath, int width, int height, bool flipY);
	private:
		ShaderProgram skyboxShader;
		GLuint textureId, irrTextureId, specularTextureId;
		VAO vao;
		VBO vbo;
		GLuint fbo, rbo;
		ShaderProgram cubemapConvertShader;
	};
}