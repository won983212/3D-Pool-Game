#include <iostream>
#include "gfx/window.h"
#include "scene.h"
#include "util/sound.h"

using namespace commoncg;

Scene scene;
int last_click = 0;
glm::vec2 last_mouse[3]; // left, middle, right mouse
Window wnd;

void init()
{
	// TODO (Debug) Turn on BGM
	// initialize sound engine
	InitSoundEngine();
	/*irrklang::ISound* sound = getSoundEngine()->play2D(SOUND_BGM, true, false, true);
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
	UIScreen::GetFontRenderer();

	glLineWidth(DefaultLineWidth);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto& i : last_mouse)
		i = {0, 0};

	scene.Init();
}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene.Render();
}

void reshape(int w, int h)
{
	glutReshapeWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void drag(int x, int y)
{
	const glm::vec2 delta = glm::vec2(x, y) - last_mouse[last_click];
	scene.MouseDrag(last_click, x, y, delta.x, delta.y);
	last_mouse[last_click] = {x, y};
}

void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)
	{
		last_click = button;
		last_mouse[button].x = x;
		last_mouse[button].y = y;
	}
	scene.Mouse(button, state, x, y);
}

void mousemove(int x, int y)
{
	scene.MouseMove(x, y);
}

void wheel(int button, int state, int x, int y)
{
	scene.MouseWheel(button, state, x, y);
}

void keyboard(unsigned char key, int x, int y)
{
	scene.Keyboard(key, x, y);
}

void update(float partial_time)
{
	scene.Update(partial_time, wnd.GetFps());
}

int main(int argc, char* argv[])
{
	wnd.Init("Pocket ball (CG Term Project)", &argc, argv);
	wnd.Create(init, render, true);
	wnd.SetMouseDragFunc(drag);
	wnd.SetMouseFunc(mouse);
	wnd.SetKeyboardFunc(keyboard);
	wnd.SetMouseMotionFunc(mousemove);
	wnd.SetMouseWheelFunc(wheel);
	wnd.SetReshapeFunc(reshape);
	wnd.SetIdleFunc(update);
	wnd.Loop();

	DestroySoundEngine();
	return 0;
}
