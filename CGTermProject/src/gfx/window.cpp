#include <iostream>
#include <time.h>
#include <functional>
#include "gfx.h"
#include "window.h"
#include "../util/util.h"

using namespace commoncg;

Window* Window::inst = NULL;

Window::Window(const char* title, int* argcp, char** argv)
	:title(title)
{
	glutInit(argcp, argv);
}

void Window::create(WindowCallback init, WindowCallback display, bool useMSAA)
{
	if (useMSAA)
	{
		glutSetOption(GLUT_MULTISAMPLE, 4);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	}
	else
	{
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	}

	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow(title);
	glewInit();
	glutIgnoreKeyRepeat(1);

	if (useMSAA)
	{
		glEnable(GL_MULTISAMPLE);
		glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	}

	init();
	this->render = display;
	inst = this;

	glutDisplayFunc(renderWrapper);
	glutIdleFunc(update);
}

void Window::loop() const
{
	glutMainLoop();
	if (destroy != NULL)
		destroy();
}

void Window::setDestroyFunc(WindowCallback destory)
{
	this->destroy = destroy;
}

void Window::setReshapeFunc(ReshapeCallback reshape) const
{
	glutReshapeFunc(reshape);
}

void Window::setKeyboardFunc(KeyboardCallback keyEvent) const
{
	glutKeyboardFunc(keyEvent);
}

void Window::setKeyboardUpFunc(KeyboardCallback keyEvent) const
{
	glutKeyboardUpFunc(keyEvent);
}

void Window::setMouseFunc(MouseCallback mouseEvent) const
{
	glutMouseFunc(mouseEvent);
}

void Window::setMouseMotionFunc(MouseMotionCallback mouseEvent) const
{
	glutPassiveMotionFunc(mouseEvent);
}

void Window::setMouseWheelFunc(MouseCallback mouseEvent) const
{
	glutMouseWheelFunc(mouseEvent);
}

void Window::setMouseDragFunc(MouseMotionCallback mouseEvent) const
{
	glutMotionFunc(mouseEvent);
}

void Window::setIdleFunc(UpdateCallback idle)
{
	this->idle = idle;
}

void Window::renderWrapper()
{
	inst->render();
	glutSwapBuffers();
}

void Window::update()
{
	long lastFrame = inst->lastFrame;
	long lastFPSFrame = inst->lastFPSCountFrame;
	long currentTime = clock();

	if (currentTime - lastFPSFrame >= CLOCKS_PER_SEC)
	{
		std::cout << "FPS: " << inst->fps << std::endl;
		inst->fps = 0;
		inst->lastFPSCountFrame = currentTime;
	}
	else
	{
		inst->fps++;
	}

	inst->partialTime = lastFrame == 0 ? 0 : (float)(currentTime - lastFrame) / CLOCKS_PER_SEC;
	inst->lastFrame = currentTime;

	if (inst->idle != NULL)
		inst->idle(inst->partialTime);

	glutPostRedisplay();
}

float Window::getPartialTime() const
{
	return partialTime;
}