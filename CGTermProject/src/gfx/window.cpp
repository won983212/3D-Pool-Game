#include <iostream>
#include <ctime>
#include <functional>
#include "gfx.h"
#include "window.h"
#include "../util/util.h"

using namespace commoncg;

Window* Window::inst_ = nullptr;

void Window::Init(const char* title, int* argcp, char** argv)
{
	this->title_ = title;
	glutInit(argcp, argv);
}

void Window::Create(WindowCallback init, WindowCallback render, bool use_msaa)
{
	if (use_msaa)
	{
		glutSetOption(GLUT_MULTISAMPLE, 4);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_MULTISAMPLE);
	}
	else
	{
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	}

	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow(title_);
	glewInit();
	glutIgnoreKeyRepeat(1);

	if (use_msaa)
	{
		glEnable(GL_MULTISAMPLE);
		glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
	}

	init();
	this->render_ = render;
	inst_ = this;

	glutDisplayFunc(RenderWrapper);
	glutIdleFunc(Update);
}

void Window::Loop() const
{
	glutMainLoop();
	if (destroy_ != nullptr)
		destroy_();
}

void Window::SetDestroyFunc(WindowCallback destory)
{
	this->destroy_ = destory;
}

void Window::SetReshapeFunc(ReshapeCallback reshape) const
{
	glutReshapeFunc(reshape);
}

void Window::SetKeyboardFunc(KeyboardCallback key_event) const
{
	glutKeyboardFunc(key_event);
}

void Window::SetKeyboardUpFunc(KeyboardCallback key_event) const
{
	glutKeyboardUpFunc(key_event);
}

void Window::SetMouseFunc(MouseCallback mouse_event) const
{
	glutMouseFunc(mouse_event);
}

void Window::SetMouseMotionFunc(MouseMotionCallback mouse_event) const
{
	glutPassiveMotionFunc(mouse_event);
}

void Window::SetMouseWheelFunc(MouseCallback mouse_event) const
{
	glutMouseWheelFunc(mouse_event);
}

void Window::SetMouseDragFunc(MouseMotionCallback mouse_event) const
{
	glutMotionFunc(mouse_event);
}

void Window::SetIdleFunc(UpdateCallback idle)
{
	this->idle_ = idle;
}

void Window::RenderWrapper()
{
	inst_->render_();
	glutSwapBuffers();
}

void Window::Update()
{
	const long last_frame = inst_->last_frame_;
	const long last_fps_frame = inst_->last_fps_count_frame_;
	const long current_time = clock();

	if (current_time - last_fps_frame >= CLOCKS_PER_SEC)
	{
		inst_->fps_ = inst_->fps_count_;
		inst_->fps_count_ = 1;
		inst_->last_fps_count_frame_ = current_time;
	}
	else
	{
		inst_->fps_count_++;
	}

	inst_->partial_time_ = last_frame == 0 ? 0 : static_cast<float>(current_time - last_frame) / CLOCKS_PER_SEC;
	inst_->last_frame_ = current_time;

	if (inst_->idle_ != nullptr)
		inst_->idle_(inst_->partial_time_);

	glutPostRedisplay();
}

float Window::GetPartialTime() const
{
	return partial_time_;
}

int Window::GetFps() const
{
	return fps_;
}
