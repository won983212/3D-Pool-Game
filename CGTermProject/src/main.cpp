#include <iostream>
#include <math.h>
#include "gfx/window.h"
#include "scene.h"
#include "util/sound.h"

using namespace commoncg;

Scene scene;
int lastClick = 0;
glm::vec2 lastMouse[3]; // left, middle, right mouse
Window wnd;

void init()
{
    // TODO (Debug) Turn on BGM
    // initialize sound engine, play bgm
    getSoundEngine();
    /*irrklang::ISound* sound = getSoundEngine()->play2D("res/sound/bgm.wav", true, false, true);
    if (!sound)
    {
        std::cout << "Warning: Can't play BGM" << std::endl;
    }
    else
    {
        sound->setVolume(0.2f);
        sound->drop();
    }*/

    // initialize font
    UIScreen::getFontRenderer();

    scene.init();
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < 3; i++)
        lastMouse[i] = {0, 0};
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    scene.render();
}

void reshape(int w, int h)
{
    glutReshapeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void drag(int x, int y)
{
    glm::vec2 mouse = glm::vec2(x, y);
    glm::vec2 delta = mouse - lastMouse[lastClick];
    scene.mouseDrag(lastClick, x, y, delta.x, delta.y);
    lastMouse[lastClick] = { x, y };
}

void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN)
    {
        lastClick = button;
        lastMouse[button].x = x;
        lastMouse[button].y = y;
    }
    scene.mouse(button, state, x, y);
}

void mousemove(int x, int y)
{
    scene.mouseMove(x, y);
}

void wheel(int button, int state, int x, int y)
{
    scene.mouseWheel(button, state, x, y);
}

void keyboard(unsigned char key, int x, int y)
{
    scene.keyboard(key, x, y);
}

void update(float partialTime)
{
    scene.update(partialTime, wnd.getFPS());
}

int main(int argc, char* argv[])
{
    wnd.init("Pocket ball (CG Term Project)", &argc, argv);
	wnd.create(init, render, true);
    wnd.setMouseDragFunc(drag);
    wnd.setMouseFunc(mouse);
    wnd.setKeyboardFunc(keyboard);
    wnd.setMouseMotionFunc(mousemove);
    wnd.setMouseWheelFunc(wheel);
    wnd.setReshapeFunc(reshape);
    wnd.setIdleFunc(update);
	wnd.loop();
    destroySoundEngine();
	return 0;
}