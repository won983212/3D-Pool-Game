#pragma once
#include <cstddef>

namespace commoncg
{
	typedef void (*WindowCallback)();
	typedef void (*RenderCallback)(float partialTime);
	typedef void (*ReshapeCallback)(int w, int h);
	typedef void (*KeyboardCallback)(unsigned char key, int x, int y);
	typedef void (*MouseCallback)(int button, int state, int x, int y);
	typedef void (*MouseMotionCallback)(int x, int y);

	class Window
	{
	public:
		Window(const char* title, int* argcp, char** argv);
		void create(WindowCallback init, RenderCallback render);
		void setDestroyFunc(WindowCallback destory);
		void setReshapeFunc(ReshapeCallback reshape) const;
		void setKeyboardFunc(KeyboardCallback keyEvent) const;
		void setMouseFunc(MouseCallback mouseEvent) const;
		void setMouseDragFunc(MouseMotionCallback mouseEvent) const;
		void setMouseWheelFunc(MouseCallback mouseEvent) const;
		float getPartialTime() const;
		void loop() const;
	private:
		static void renderWrapper();
		static void update();
	private:
		static Window* inst;
		WindowCallback destroy = NULL;
		RenderCallback render = NULL;
		const char* title;
		float partialTime = 0;
		long lastFrame = 0;
		long lastFPSCountFrame = 0;
		int fps = 0;
	};
}