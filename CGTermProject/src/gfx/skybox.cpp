#define SKYBOX_TEXTURE_COUNT 6

#include <iostream>
#include <string>
#include "../util/stb_image.h"
#include "../util/util.h"
#include "skybox.h"

using namespace commoncg;

const float size = 500.0f;

const std::string skyboxTexNames[] = {
    "right.jpg",
    "left.jpg",
    "top.jpg",
    "bottom.jpg",
    "front.jpg",
    "back.jpg"
};

const float skyboxVerts[] = {
    -size,  size, -size,
    -size, -size, -size,
     size, -size, -size,
     size, -size, -size,
     size,  size, -size,
    -size,  size, -size,

    -size, -size,  size,
    -size, -size, -size,
    -size,  size, -size,
    -size,  size, -size,
    -size,  size,  size,
    -size, -size,  size,

     size, -size, -size,
     size, -size,  size,
     size,  size,  size,
     size,  size,  size,
     size,  size, -size,
     size, -size, -size,

    -size, -size,  size,
    -size,  size,  size,
     size,  size,  size,
     size,  size,  size,
     size, -size,  size,
    -size, -size,  size,

    -size,  size, -size,
     size,  size, -size,
     size,  size,  size,
     size,  size,  size,
    -size,  size,  size,
    -size,  size, -size,

    -size, -size, -size,
    -size, -size,  size,
     size, -size, -size,
     size, -size, -size,
    -size, -size,  size,
     size, -size,  size
};


static GLuint loadCubemap(const char* textureDirPath)
{
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    std::string dir(textureDirPath);
    while (dir.back() == '/' || dir.back() == '\\')
        dir.pop_back();

    int width, height, nChannels;
    for (int i = 0; i < SKYBOX_TEXTURE_COUNT; i++)
    {
        unsigned char* data = stbi_load((dir + "/" + skyboxTexNames[i]).c_str(), &width, &height, &nChannels, 0);
        if (data)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            std::cout << "Error: Failed to load skybox texture." << std::endl;
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return id;
}

void Skybox::load(const char* textureDirPath, const char* skyboxVertShader, const char* skyboxFragShader)
{
    textureId = loadCubemap(textureDirPath);

    skyboxShader.addShader(skyboxVertShader, GL_VERTEX_SHADER);
    skyboxShader.addShader(skyboxFragShader, GL_FRAGMENT_SHADER);
    skyboxShader.load();

    vao.create();
    vbo.create();
    vao.bind();
    vbo.buffer(sizeof(skyboxVerts), skyboxVerts);
    vao.attr(0, 3, GL_FLOAT, SIZEOF(float, 3), 0);
    VAO::unbind();
}

// TODO Skybox is too small
void Skybox::render(const ShaderProgram& shader) const
{
    skyboxShader.use();
    glDepthFunc(GL_LEQUAL);

    vao.bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    VAO::unbind();
    glDepthFunc(GL_LESS);
    shader.use();
}