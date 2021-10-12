#include <iostream>
#include <string>
#include "../util/util.h"
#include "texture.h"
#include "skybox.h"

using namespace commoncg;

// cubemap projection and views(6 faces)
const glm::mat4 projection = glm::perspective(DEGTORAD(90.0f), 1.0f, 0.1f, 10.0f);
const glm::mat4 views[] =
{
    glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
    glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))
};

const int skyboxTexSize = 1024;
const int irradianceTexSize = 32;
const int specularTexSize = 256;
const float skyboxVertices[] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

GLuint Skybox::loadEquirectangularMap(const char* texturePath, int width, int height, bool flipY)
{
    if (!vao.isCreated())
    {
        std::cout << "Error Skybox: Loading the irradiance texture should be executed after skybox is created: " << texturePath << std::endl;
        return 0;
    }

    Texture texture;
    texture.loadImage(texturePath);

    // prepare framebuffer and depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // prepare cubemap
    int mipmapLevel = texture.getCountMipmap();
    GLuint cubemapTextureId;
    glGenTextures(1, &cubemapTextureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureId);

    for (int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, mipmapLevel > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (mipmapLevel > 1)
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    // prepare shader for conversion.
    cubemapConvertShader.use();
    cubemapConvertShader.setUniform("flipY", flipY);

    glActiveTexture(GL_TEXTURE0);
    texture.bind();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    for (int lv = 0; lv < mipmapLevel; lv++)
    {
        unsigned int w = width * std::pow(0.5, lv);
        unsigned int h = height * std::pow(0.5, lv);
        if (lv > 0)
        {
            glBindRenderbuffer(GL_RENDERBUFFER, rbo);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
        }

        glViewport(0, 0, w, h);
        cubemapConvertShader.setUniform("lod", lv);
        for (int i = 0; i < 6; i++)
        {
            cubemapConvertShader.setUniform("view", views[i]);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapTextureId, lv);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            vao.bind();
            glDrawArrays(GL_TRIANGLES, 0, 36);
            VAO::unbind();
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return cubemapTextureId;
}

void Skybox::loadHDRSkybox(const char* hdrTexturePath)
{
    textureId = loadEquirectangularMap(hdrTexturePath, skyboxTexSize, skyboxTexSize, false);
}

void Skybox::loadDDSIrradianceMap(const char* ddsTexturePath)
{
    irrTextureId = loadEquirectangularMap(ddsTexturePath, irradianceTexSize, irradianceTexSize, true);
}

void Skybox::loadDDSSpecularMap(const char* ddsTexturePath)
{
    specularTextureId = loadEquirectangularMap(ddsTexturePath, specularTexSize, specularTexSize, true);
}

void Skybox::beginLoad()
{
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);

    // prepare cube vao, vbo
    vao.create();
    vbo.create();
    vao.bind();
    vbo.buffer(sizeof(skyboxVertices), skyboxVertices);
    vao.attr(0, 3, GL_FLOAT, SIZEOF(float, 3), 0);
    VAO::unbind();

    // prepare conversion shader.
    cubemapConvertShader.addShader("res/shader/cubemap.vert", GL_VERTEX_SHADER);
    cubemapConvertShader.addShader("res/shader/eqrtocubemap.frag", GL_FRAGMENT_SHADER);
    cubemapConvertShader.load();
    cubemapConvertShader.use();
    cubemapConvertShader.setUniform("equirectangularMap", 0);
    cubemapConvertShader.setUniform("projection", projection);

    // prepare skybox shader.
    skyboxShader.addShader("res/shader/skybox.vert", GL_VERTEX_SHADER);
    skyboxShader.addShader("res/shader/skybox.frag", GL_FRAGMENT_SHADER);
    skyboxShader.load();
    skyboxShader.use();
    skyboxShader.setUniform("skyboxMap", 0);
}

void Skybox::endLoad(const ShaderProgram& shader)
{
    if (!vao.isCreated())
    {
        std::cout << "Error Skybox: endLoad: beginLoad is not invoked!" << std::endl;
        return;
    }

    cubemapConvertShader.destroy();
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    shader.use();
    shader.setUniform("irradianceMap", 0);
    shader.setUniform("specularMap", 1);

    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
}

void Skybox::bindEnvironmentTextures() const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, irrTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, specularTextureId);
}

void Skybox::render(const ShaderProgram& shader, glm::mat4 view) const // TODO view must used by uniform block.
{
    skyboxShader.use();
    skyboxShader.setUniform("view", view);
    glDepthFunc(GL_LEQUAL);

    vao.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    VAO::unbind();
    glDepthFunc(GL_LESS);
    shader.use();
}