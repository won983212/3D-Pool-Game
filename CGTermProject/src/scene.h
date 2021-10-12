#pragma once

#include "gfx/gfx.h"
#include "gfx/vbo.h"
#include "gfx/camera.h"
#include "gfx/skybox.h"
#include "model/assetmodel.h"
#include "model/ball.h"
#include "pooltable.h"


struct LightData
{
    glm::vec4 position;
    glm::vec4 color;
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
    void render();
private:
    void updateLight();
    void updateView();
public:
    // pool table physics simulator
    PoolTable table;
    // models
    model::AssetModel modelPoolTable;
    model::Ball modelBall;
    // graphics
    commoncg::ShaderProgram shader;
    commoncg::Skybox skybox;
    commoncg::Camera cam;
    commoncg::Texture brdfLUT;
private:
    // light
    LightData lights[2];
    commoncg::VBO uboLight;
    // view
    ViewMatrixData view;
    commoncg::VBO uboView;
};