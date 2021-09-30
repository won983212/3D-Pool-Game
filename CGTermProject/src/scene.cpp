#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "scene.h"
#include "gfx/texture.h"
#include "util/util.h"

using namespace glm;
using namespace commoncg;

float lightAngle = 0;
Texture* ballTexture;
model::AssetModel modelPoolTable;

void Scene::init()
{
    ballTexture = Texture::cacheImage("res/textures/ball_10.png");
    modelBall.init();

    shader.addShader("res/shader/simple.vert", GL_VERTEX_SHADER);
    shader.addShader("res/shader/simple.frag", GL_FRAGMENT_SHADER);
    shader.load();
    shader.use();

    modelPoolTable.loadModel("res/models/untitled.obj");
    uboLight.create();
    uboView.create();

    light.direction = vec4(1.0f, -1.0f, -1.0f, 1.0f);
    light.diffuse = vec4(0.8f);
    light.ambient = vec4(0.1f);
    light.specular = vec4(1.0f);
    updateLight();

    view.view = cam.getViewMatrix();
    view.projection = perspective(DEGTORAD(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
    updateView();
}

void Scene::update(float partialTime)
{
    lightAngle += M_PI * partialTime;
    light.direction = vec4(cos(lightAngle), -1.0f, sin(lightAngle), 1.0f);
    updateLight();
    table.update(partialTime);
}

void Scene::updateLight()
{
    uboLight.bindBufferRange(UNIFORM_BINDING_LIGHT, 0, sizeof(light));
    uboLight.buffer(sizeof(light), &light);
    uboLight.unbind();
}

void Scene::updateView()
{
    uboView.bindBufferRange(UNIFORM_BINDING_VIEWMAT, 0, sizeof(view));
    uboView.buffer(sizeof(view), &view);
    uboView.unbind();
}

void Scene::render(float partialTime)
{
    view.view = cam.getViewMatrix();
    updateView();

    shader.setUniform("viewPos", cam.position);
    ballTexture->bind();

    const std::vector<Ball*> balls = table.getBalls();
    for (unsigned int i = 0; i < balls.size(); i++)
    {
        vec2 pos = balls[i]->position;
        mat4 model = mat4(1.0f);

        model = translate(model, vec3(pos.x, BALL_RADIUS, pos.y));
        model = model * glm::toMat4(balls[i]->rotation);

        shader.setUniform("model", model);
        modelBall.draw(shader);
    }

    Texture::unbind();
    shader.setUniform("model", mat4(1.0f));
    modelPoolTable.draw(shader);
}