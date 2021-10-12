#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "scene.h"
#include "gfx/texture.h"
#include "util/util.h"
#include "model/quad.h"

using namespace glm;
using namespace commoncg;

// TODO debug global variables
Texture ballTexture;
model::Material ballMaterial;

void Scene::init()
{
    // TODO (Debug) initialize
    ballTexture.loadImage("res/textures/ball_10.png");
    ballMaterial.albedo = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);
    ballMaterial.metallic = 0.1f;
    ballMaterial.roughness = 0.1f;
    ballMaterial.ao = DEFAULT_AO;
    for (int i = 0; i < 4; i++)
        ballMaterial.texIndex[i] = -1;
    ballMaterial.texIndex[(int)model::TextureType::ALBEDO] = 1;

    brdfLUT.loadImage("res/textures/brdf.png");
    modelBall.init(BALL_RADIUS);

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

    skybox.beginLoad();
    skybox.loadHDRSkybox("res/textures/skybox/skybox.hdr");
    skybox.loadDDSIrradianceMap("res/textures/skybox/irr.dds");
    skybox.loadDDSSpecularMap("res/textures/skybox/env.dds");
    skybox.endLoad(shader);

    modelPoolTable.loadModel("res/models/table.gltf");
    uboLight.create();
    uboView.create();

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

    view.view = cam.getViewMatrix();
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
    view.view = cam.getViewMatrix();
    updateView();
    shader.setUniform("camPos", cam.getEyePosition());

    // bind env maps and brdf LUT (lookup texture).
    skybox.bindEnvironmentTextures();
    glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_BRDFMAP);
    brdfLUT.bind();

    // TODO (Debug) bind ball texture
    glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_ALBEDO);
    ballTexture.bind();

    // balls
    model::bindMaterial(&ballMaterial);
    const std::vector<Ball*> balls = table.getBalls();
    for (unsigned int i = 0; i < balls.size(); i++)
    {
        vec2 pos = balls[i]->position;
        mat4 model = mat4(1.0f);

        model = translate(model, vec3(pos.x, BALL_RADIUS, pos.y));
        model = model * glm::toMat4(balls[i]->rotation);

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
    skybox.render(shader, cam.getViewMatrix());
}