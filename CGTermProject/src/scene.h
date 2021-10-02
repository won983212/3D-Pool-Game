#pragma once

#include "gfx/gfx.h"
#include "gfx/vbo.h"
#include "gfx/camera.h"
#include "gfx/skybox.h"
#include "model/assetmodel.h"
#include "model/ball.h"
#include "pooltable.h"


struct DirectionalLightData
{
    glm::vec4 direction;
    glm::vec4 diffuse;
    glm::vec4 ambient;
    glm::vec4 specular;
};

struct ViewMatrixData
{
    glm::mat4 view;
    glm::mat4 projection;
};

class Scene
{
public:
    Scene() : uboLight(GL_UNIFORM_BUFFER), uboView(GL_UNIFORM_BUFFER), 
        cam(-45, 30, -20, 14, -35), table(glm::vec2(42.0f, 22.0f)) {};
    void init();
    void update(float partialTime);
    void render(float partialTime);
private:
    void updateLight();
    void updateView();
public:
    commoncg::Camera cam;
private:
    // light
    DirectionalLightData light;
    commoncg::VBO uboLight;
    // view
    ViewMatrixData view;
    commoncg::VBO uboView;
    // pool table physics simulator
    PoolTable table;
    // models
    model::AssetModel modelPoolTable;
    model::Ball modelBall;
    // graphics
    commoncg::ShaderProgram shader;
    commoncg::Skybox skybox;
};