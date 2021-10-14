#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "scene.h"
#include "gfx/texture.h"
#include "util/util.h"
#include "model/quad.h"
#include "ui/button.h"

using namespace glm;
using namespace commoncg;


const float PRJ_FOV = DEGTORAD(45.0f);
const float PRJ_ASPECT = (float)SCREEN_WIDTH / SCREEN_HEIGHT;
const float PRJ_NEAR = 0.1f;
const float PRJ_FAR = 100.0f;

const model::Material BALL_MATERIAL = 
{
    glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),  // albedo
    0.1f,           // metallic
    0.1f,           // roughness
    DEFAULT_AO,     // ao
    0, -1, -1, -1   // texIndex
};

// TODO (Debug) Test Beam Shader
ShaderProgram beamShader;
VAO beamVao;
VBO beamVbo;


void Scene::init()
{
    // TODO (Debug) Test Beam Shader
    float initialVert[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    beamVao.create();
    beamVbo.create();
    beamVao.use();
    beamVbo.buffer(SIZEOF(float, 6), initialVert, GL_DYNAMIC_DRAW);
    beamVao.attr(0, 3, GL_FLOAT, SIZEOF(float, 3), 0);
    VAO::unbind();
    beamShader.addShader("res/shader/test.vert", GL_VERTEX_SHADER);
    beamShader.addShader("res/shader/test.frag", GL_FRAGMENT_SHADER);
    beamShader.load();
    beamShader.use();
    beamShader.setUniform("color", glm::vec3(1.0f, 0.0f, 0.0f));


    // initalize graphic variables
    brdfLUT.loadImage("res/texture/brdf.png");
    uboLight.create();
    uboView.create();

    // prepare ball textures
    for (int i = 0; i < BALL_TEXTURE_COUNT; i++)
        ballTextures[i] = Texture::cacheImage(("res/texture/ball/ball_" + std::to_string(i) + ".png").c_str());

    // model
    modelPoolTable.loadModel("res/model/pooltable.gltf");
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

    // gui screen
    ui.init();

    // skybox
    skybox.beginLoad();
    skybox.loadHDRSkybox("res/texture/skybox/skybox.hdr");
    skybox.loadDDSIrradianceMap("res/texture/skybox/irr.dds");
    skybox.loadDDSSpecularMap("res/texture/skybox/env.dds");
    skybox.endLoad();

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
    view.projection = perspective(PRJ_FOV, PRJ_ASPECT, PRJ_NEAR, PRJ_FAR);
    updateView();
}

void Scene::update(float partialTime, int fps)
{
    table.update(partialTime);
    ui.fpsLabel->setText(std::wstring(L"FPS: ") + std::to_wstring(fps));
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

// TODO Back plane�� material���� ���� rendering��
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
    modelPoolTable.draw();

    // skybox
    skybox.render(view.view);

    // gui
    ui.render();

    // TODO (Debug) Beam rendering
    beamShader.use();
    beamVao.use();
    glDrawArrays(GL_LINES, 0, 2);
    VAO::unbind();
    shader.use();
}

MouseRay Scene::calculateMouseRay(int mouseX, int mouseY)
{
    MouseRay ray;

    float sx = mouseX * 2.0f / SCREEN_WIDTH - 1.0f;
    float sy = mouseY * 2.0f / SCREEN_HEIGHT - 1.0f;
    float halfHeight = tan(PRJ_FOV / 2) * PRJ_NEAR;

    sx *= PRJ_ASPECT * halfHeight;
    sy *= halfHeight;

    ray.position = glm::vec3(cam.getEyePosition());
    ray.direction = -cam.getUp() * sy + cam.getRight() * sx;

    ray.position = ray.position + ray.direction;
    ray.direction += glm::normalize(cam.getFront()) * PRJ_NEAR;

    return ray;
}

void Scene::mouse(int button, int state, int x, int y)
{
    ui.mouse(button, state, x, y);
}

void Scene::mouseDrag(int button, int x, int y, int dx, int dy)
{
    if (button == GLUT_RIGHT_BUTTON && ui.getCurrentScreen() == 2)
    {
        cam.yaw += dx / 8.0f;
        cam.pitch += dy / 8.0f;
        cam.update();
    }
    ui.mouseDrag(x, y);
}

void Scene::mouseWheel(int button, int state, int x, int y)
{
    if (ui.getCurrentScreen() == 2)
    {
        if (state > 0)
            cam.zoom -= 0.5f;
        else
            cam.zoom += 0.5f;
        cam.update();
    }
    ui.mouseWheel(button, state, x, y);
}

void Scene::mouseMove(int x, int y)
{
    ui.mouseMove(x, y);
    MouseRay calc = calculateMouseRay(x, y);
    glm::vec3 verts[] = { calc.position, calc.position + 1000.0f * calc.direction };
    beamVao.use();
    beamVbo.buffer(SIZEOF(float, 6), verts, GL_DYNAMIC_DRAW);
    beamVbo.unbind();
    VAO::unbind();
}