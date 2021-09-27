#include <iostream>
#include <math.h>
#include "gfx/window.h"
#include "gfx/shader.h"
#include "gfx/model.h"
#include "gfx/camera.h"
#include "util/util.h"

ShaderProgram shader;
model::Model myModel;
Camera cam(0, 0, -3, -90, 0);

glm::vec2 lastMouse(0, 0);


void init()
{
    myModel.loadModel("res/models/untitled.obj");

	shader.addShader("res/shader/simple.vert", GL_VERTEX_SHADER);
	shader.addShader("res/shader/simple.frag", GL_FRAGMENT_SHADER);
	shader.load();
	shader.use();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void render(float partialTime)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = cam.getViewMatrix();
    glm::mat4 projection = glm::perspective(DEGTORAD(45.0f), (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 100.0f);

    shader.setUniform("p", projection);
    shader.setUniform("v", view);
    shader.setUniform("m", model);
    shader.setUniform("viewPos", cam.position);

    myModel.Draw(shader);
}

void destroy()
{
	shader.destroy();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void drag(int x, int y)
{
    glm::vec2 mouse = glm::vec2(x, y);
    glm::vec2 delta = mouse - lastMouse;

    cam.yaw += delta.x / 8.0f;
    cam.pitch += -delta.y / 8.0f;
    cam.update();

    lastMouse.x = x;
    lastMouse.y = y;
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        lastMouse.x = x;
        lastMouse.y = y;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    const float speed = 0.8f;
    switch (key)
    {
    case 'w':
        cam.position += speed * cam.getFront();
        cam.update();
        break;
    case 's':
        cam.position += -speed * cam.getFront();
        cam.update();
        break;
    case 'a':
        cam.position += -speed * cam.getRight();
        cam.update();
        break;
    case 'd':
        cam.position += speed * cam.getRight();
        cam.update();
        break;
    }
}

int main(int argc, char* argv[])
{
    Window wnd("CG Term Project", &argc, argv);
	wnd.create(init, render);
    wnd.setMouseDragFunc(drag);
    wnd.setMouseFunc(mouse);
    wnd.setKeyboardFunc(keyboard);
    wnd.setReshapeFunc(reshape);
	wnd.setDestroyFunc(destroy);
	wnd.loop();
	return 0;
}