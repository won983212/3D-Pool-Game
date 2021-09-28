#pragma once

#include "gfx/gfx.h"
#include "gfx/vbo.h"

struct DirectionalLightData
{
    glm::vec4 direction;
    glm::vec4 diffuse;
    glm::vec4 ambient;
    glm::vec4 specular;
};

class DirectionalLight
{
public:
    DirectionalLight();
    void create();
    void update() const;
public:
    DirectionalLightData data;
private:
    commoncg::VBO uboLight;
};