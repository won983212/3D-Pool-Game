#pragma once

#include "gfx/gfx.h"
#include "gfx/vbo.h"
#include "gfx/camera.h"
#include "gfx/skybox.h"
#include "ui/screenmenu.h"
#include "model/assetmodel.h"
#include "model/ball.h"
#include "pooltable.h"

#define BALL_TEXTURE_COUNT 16


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
            cam(0, 30, 10), table(glm::vec2(5.5f, 10.8f)) {};
    void init();
    void update(float partialTime);
    void render();
    void mouse(int button, int state, int x, int y);
    void mouseWheel(int button, int state, int x, int y);
    void mouseMove(int x, int y);
    void mouseDrag(int x, int y);
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
private:
    // light
    LightData lights[3];
    commoncg::VBO uboLight;
    // view
    ViewMatrixData view;
    commoncg::VBO uboView;
    // graphics
    commoncg::Texture brdfLUT;
    commoncg::Texture* ballTextures[BALL_TEXTURE_COUNT];
    ScreenMenu ui;
};