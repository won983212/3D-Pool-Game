#include <iostream>
#include <math.h>
#include "gfx/window.h"
#include "scene.h"

using namespace commoncg;

Scene scene;
glm::vec2 lastMouse(0, 0);
bool keyState[256];

void init()
{
    scene.init();
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.render();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

void drag(int x, int y)
{
    glm::vec2 mouse = glm::vec2(x, y);
    glm::vec2 delta = mouse - lastMouse;

    scene.cam.yaw += delta.x / 8.0f;
    scene.cam.pitch += -delta.y / 8.0f;
    scene.cam.update();

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
    keyState[key] = true;
}

void keyboardUp(unsigned char key, int x, int y)
{
    keyState[key] = false;
}

void update(float partialTime)
{
    const float speed = 10.0f * partialTime;
    const float xyzSpeed = 0.1f;

    scene.update(partialTime);

    if (keyState['w'])
    {
        scene.cam.position += speed * scene.cam.getFront();
        scene.cam.update();
    }
    if (keyState['s'])
    {
        scene.cam.position += -speed * scene.cam.getFront();
        scene.cam.update();
    }
    if (keyState['a'])
    {
        scene.cam.position += -speed * scene.cam.getRight();
        scene.cam.update();
    }
    if (keyState['d'])
    {
        scene.cam.position += speed * scene.cam.getRight();
        scene.cam.update();
    }
}

int main(int argc, char* argv[])
{
    memset(keyState, 0, sizeof(keyState));
    Window wnd("Pocket ball (CG Term Project)", &argc, argv);
	wnd.create(init, render, true);
    wnd.setMouseDragFunc(drag);
    wnd.setMouseFunc(mouse);
    wnd.setKeyboardFunc(keyboard);
    wnd.setKeyboardUpFunc(keyboardUp);
    wnd.setReshapeFunc(reshape);
    wnd.setIdleFunc(update);
	wnd.loop();
	return 0;
}