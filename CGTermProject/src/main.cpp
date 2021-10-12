#include <iostream>
#include <math.h>
#include "gfx/window.h"
#include "scene.h"

using namespace commoncg;

Scene scene;
glm::vec2 lastMouse(0, 0);

void init()
{
    scene.init();
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glEnable(GL_DEPTH_TEST);
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
    scene.cam.pitch += delta.y / 8.0f;
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

void wheel(int button, int state, int x, int y)
{
    if (state > 0)
        scene.cam.zoom -= 0.5f;
    else
        scene.cam.zoom += 0.5f;
    scene.cam.update();
}

void update(float partialTime)
{
    const float speed = 10.0f * partialTime;
    const float xyzSpeed = 0.1f;
    scene.update(partialTime);
}

int main(int argc, char* argv[])
{
    Window wnd("Pocket ball (CG Term Project)", &argc, argv);
	wnd.create(init, render, true);
    wnd.setMouseDragFunc(drag);
    wnd.setMouseFunc(mouse);
    wnd.setMouseWheelFunc(wheel);
    wnd.setReshapeFunc(reshape);
    wnd.setIdleFunc(update);
	wnd.loop();
	return 0;
}