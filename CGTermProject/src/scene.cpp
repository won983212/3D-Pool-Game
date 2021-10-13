#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "scene.h"
#include "gfx/texture.h"
#include "util/util.h"
#include "model/quad.h"

using namespace glm;
using namespace commoncg;

const model::Material BALL_MATERIAL = 
{
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),  // albedo
    0.1f,           // metallic
    0.1f,           // roughness
    DEFAULT_AO,     // ao
    0, -1, -1, -1   // texIndex
};

void Scene::init()
{
    // initalize graphic variables
    brdfLUT.loadImage("res/textures/brdf.png");
    uboLight.create();
    uboView.create();

    // prepare ball textures
    for (int i = 0; i < BALL_TEXTURE_COUNT; i++)
        ballTextures[i] = Texture::cacheImage(("res/textures/ball_" + std::to_string(i) + ".png").c_str());

    // model
    modelPoolTable.loadModel("res/models/pooltable.gltf");
    modelBall.init(BALL_RADIUS);

    // shader
    shader.addShader("res/shader/pbr.vert", GL_VERTEX_SHADER);
    shader.addShader("res/shader/pbr.frag", GL_FRAGMENT_SHADER);
    shader.load();
    shader.use();
    shader.setUniform("texture_albedo", PBR_TEXTURE_INDEX_ALBEDO);
    shader.setUniform("texture_metallic", PBR_TEXTURE_INDEX_METALLIC);
    shader.setUniform("texture_roughness", PBR_TEXTURE_INDEX_ROUGHNESS);
    shader.setUniform("texture_normal", PBR_TEXTURE_INDEX_NORMAL);
    shader.setUniform("irradianceMap", PBR_TEXTURE_INDEX_IRRADIANCEMAP);
    shader.setUniform("specularMap", PBR_TEXTURE_INDEX_SPECULARMAP);
    shader.setUniform("brdfMap", PBR_TEXTURE_INDEX_BRDFMAP);

    // skybox
    skybox.beginLoad();
    skybox.loadHDRSkybox("res/textures/skybox/skybox.hdr");
    skybox.loadDDSIrradianceMap("res/textures/skybox/irr.dds");
    skybox.loadDDSSpecularMap("res/textures/skybox/env.dds");
    skybox.endLoad(shader);

    // lights
    LightData light;
    light.position = vec4(0.0f, 3.0f, 5.0f, 1.0f);
    light.color = vec4(50);
    lights[0] = light;

    light.position = vec4(0.0f, 3.0f, 0.0f, 1.0f);
    light.color = vec4(50);
    lights[1] = light;

    light.position = vec4(0.0f, 3.0f, -5.0f, 1.0f);
    light.color = vec4(50);
    lights[2] = light;
    updateLight();

    // camera (view)
    cam.getViewMatrix(&view.view);
    view.projection = perspective(DEGTORAD(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);
    updateView();
}

void Scene::update(float partialTime)
{
    table.update(partialTime);
}

void Scene::updateLight()
{
    uboLight.bindBufferRange(UNIFORM_BINDING_LIGHT, 0, sizeof(lights));
    uboLight.buffer(sizeof(lights), &lights);
    uboLight.unbind();
}

void Scene::updateView()
{
    uboView.bindBufferRange(UNIFORM_BINDING_VIEWMAT, 0, sizeof(view));
    uboView.buffer(sizeof(view), &view);
    uboView.unbind();
}

void Scene::render()
{
    // update only when cam has changed.
    if (cam.getViewMatrix(&view.view))
        updateView();
    shader.setUniform("camPos", cam.getEyePosition());

    // bind env maps and brdf LUT (lookup texture).
    skybox.bindEnvironmentTextures();
    glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_BRDFMAP);
    brdfLUT.bind();

    // balls
    model::bindMaterial(&BALL_MATERIAL);
    const std::vector<Ball*> balls = table.getBalls();
    for (unsigned int i = 0; i < balls.size(); i++)
    {
        vec2 pos = balls[i]->position;
        mat4 model = mat4(1.0f);

        model = translate(model, vec3(pos.x, BALL_RADIUS, pos.y));
        model = model * glm::toMat4(balls[i]->rotation);

        glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_ALBEDO);
        ballTextures[i]->bind();
        shader.setUniform("model", model);
        modelBall.draw();
    }
    Texture::unbind();

    // render pool table
    mat4 model = mat4(1.0f);
    model = scale(model, vec3(5.0f));
    model = rotate(model, DEGTORAD(90.0f), vec3(1, 0, 0));
    shader.setUniform("model", model);
    modelPoolTable.draw(shader);

    // skybox
    skybox.render(shader, view.view);
}