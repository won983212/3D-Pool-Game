#pragma once

#include "gfx/gfx.h"
#include "gfx/vbo.h"
#include "gfx/camera.h"
#include "gfx/skybox.h"
#include "guiscreen.h"
#include "model/assetmodel.h"
#include "model/ball.h"
#include "pooltable.h"
#include "util/util.h"

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

struct MouseRay
{
    glm::vec3 position;
    glm::vec3 direction;
};

enum class CueMode
{
    INVISIBLE, ROTATION, PUSHING
};

class CueTransform
{
public:
    glm::mat4 getModelMatrix()
    {
        return modelMat;
    }
    void update()
    {
        modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(position.x, 0.16f, position.y)); // location
        modelMat = glm::rotate(modelMat, rotation, glm::vec3(0, 1, 0)); // rotation
        modelMat = glm::translate(modelMat, glm::vec3(0.0f, 0.0f, pushAmount)); // push model
        modelMat = glm::rotate(modelMat, DEGTORAD(90.0f), glm::vec3(1, 0, 0));
        modelMat = glm::scale(modelMat, glm::vec3(0.05f));
    }
public:
    glm::vec2 position = glm::vec2(0);
    float rotation = 0;
    float pushAmount = 0;
    CueMode mode = CueMode::ROTATION;
private:
    glm::mat4 modelMat = glm::mat4(1.0f);
};

class Scene
{
public:
    Scene() : uboLight(GL_UNIFORM_BUFFER), uboView(GL_UNIFORM_BUFFER), 
            cam(0, 30, 10), table(glm::vec2(5.5f, 10.8f)) {};
    void init();
    void update(float partialTime, int fps);
    void render();
    void mouse(int button, int state, int x, int y);
    void mouseWheel(int button, int state, int x, int y);
    void mouseMove(int x, int y);
    void mouseDrag(int button, int x, int y, int dx, int dy);
private:
    void updateLight();
    void updateView();
    MouseRay calculateMouseRay(int mouseX, int mouseY);
    void hitWhiteBall();
private:
    // camera ball view
    bool isBallView = false;
    // light
    LightData lights[3];
    commoncg::VBO uboLight;
    // view
    ViewMatrixData view;
    commoncg::VBO uboView;
    // pool table physics simulator
    PoolTable table;
    // models
    CueTransform cueTransform;
    model::AssetModel modelPoolTable;
    model::AssetModel modelCue;
    model::Ball modelBall;
    // graphics
    commoncg::ShaderProgram shader;
    commoncg::Skybox skybox;
    commoncg::Camera cam;
    commoncg::Texture brdfLUT;
    commoncg::Texture* ballTextures[BALL_TEXTURE_COUNT];
    GUIScreen ui;
};