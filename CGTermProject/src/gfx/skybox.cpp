#include <iostream>
#include "../util/util.h"
#include "texture.h"
#include "skybox.h"

using namespace commoncg;

// cubemap projection and views(6 faces)
const glm::mat4 Projection = glm::perspective(DEGTORAD(90.0f), 1.0f, 0.1f, 10.0f);
const glm::mat4 Views[] =
{
	lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
	lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
	lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
	lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};

constexpr int SkyboxTexSize = 1024;
constexpr int IrradianceTexSize = 32;
constexpr int SpecularTexSize = 256;

constexpr float SkyboxVertices[] = {
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};


GLuint Skybox::LoadEquirectangularMap(const char* texture_path, int width, int height, const bool flip_y) const
{
	if (!vao_.IsCreated())
	{
		std::cout << "Error Skybox: Loading the irradiance texture should be executed after skybox is created: " << texture_path << std::endl;
		return 0;
	}

	Texture texture;
	texture.LoadTextureImage(texture_path);

	// prepare framebuffer and depth Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_);

	// prepare cubemap
	const int mipmap_level = texture.GetCountMipmap();
	GLuint cubemap_texture_id;
	glGenTextures(1, &cubemap_texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_texture_id);

	for (int i = 0; i < 6; i++)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipmap_level > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (mipmap_level > 1)
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// prepare shader for conversion.
	cubemap_convert_shader_.Use();
	cubemap_convert_shader_.SetUniform("flipY", flip_y);

	glActiveTexture(GL_TEXTURE0);
	texture.Bind();
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

	for (int lv = 0; lv < mipmap_level; lv++)
	{
		unsigned int w = width * std::pow(0.5, lv);
		unsigned int h = height * std::pow(0.5, lv);
		if (lv > 0)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
		}

		glViewport(0, 0, w, h);
		cubemap_convert_shader_.SetUniform("lod", lv);
		for (int i = 0; i < 6; i++)
		{
			cubemap_convert_shader_.SetUniform("view", Views[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemap_texture_id, lv);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			vao_.Use();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			VAO::Unbind();
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return cubemap_texture_id;
}

void Skybox::LoadHDRSkybox(const char* hdr_texture_path)
{
	texture_id_ = LoadEquirectangularMap(hdr_texture_path, SkyboxTexSize, SkyboxTexSize, false);
}

void Skybox::LoadDDSIrradianceMap(const char* dds_texture_path)
{
	irr_texture_id_ = LoadEquirectangularMap(dds_texture_path, IrradianceTexSize, IrradianceTexSize, true);
}

void Skybox::LoadDDSSpecularMap(const char* dds_texture_path)
{
	specular_texture_id_ = LoadEquirectangularMap(dds_texture_path, SpecularTexSize, SpecularTexSize, true);
}

void Skybox::BeginLoad()
{
	glGenFramebuffers(1, &fbo_);
	glGenRenderbuffers(1, &rbo_);

	// prepare cube vao, vbo
	vao_.Create();
	vbo_.Create();
	vao_.Use();
	vbo_.Buffer(sizeof SkyboxVertices, SkyboxVertices);
	vao_.Attrib(0, 3, GL_FLOAT, SIZEOF(float, 3), 0);
	VAO::Unbind();

	ShaderProgram::Push();

	// prepare conversion shader.
	cubemap_convert_shader_.AddShader("res/shader/cubemap.vert", GL_VERTEX_SHADER);
	cubemap_convert_shader_.AddShader("res/shader/eqrtocubemap.frag", GL_FRAGMENT_SHADER);
	cubemap_convert_shader_.Load();
	cubemap_convert_shader_.Use();
	cubemap_convert_shader_.SetUniform("equirectangularMap", 0);
	cubemap_convert_shader_.SetUniform("projection", Projection);

	// prepare skybox shader.
	skybox_shader_.AddShader("res/shader/skybox.vert", GL_VERTEX_SHADER);
	skybox_shader_.AddShader("res/shader/skybox.frag", GL_FRAGMENT_SHADER);
	skybox_shader_.Load();
	skybox_shader_.Use();
	skybox_shader_.SetUniform("skyboxMap", 0);
}

void Skybox::EndLoad() const
{
	if (!vao_.IsCreated())
	{
		std::cout << "Error Skybox: endLoad: beginLoad is not invoked!" << std::endl;
		return;
	}

	cubemap_convert_shader_.Destroy();
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glDeleteFramebuffers(1, &fbo_);
	glDeleteRenderbuffers(1, &rbo_);
	ShaderProgram::Pop();
}

void Skybox::BindEnvironmentTextures() const
{
	glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_IRRADIANCEMAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irr_texture_id_);
	glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_SPECULARMAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, specular_texture_id_);
}

void Skybox::Render(glm::mat4 view) const
{
	ShaderProgram::Push();
	skybox_shader_.Use();
	glDepthFunc(GL_LEQUAL);

	vao_.Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id_);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	VAO::Unbind();
	glDepthFunc(GL_LESS);
	ShaderProgram::Pop();
}
