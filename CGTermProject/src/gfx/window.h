#pragma once

namespace commoncg
{
	using WindowCallback = void(*)();
	using UpdateCallback = void(*)(float partial_time);
	using ReshapeCallback = void(*)(int w, int h);
	using KeyboardCallback = void(*)(unsigned char key, int x, int y);
	using MouseCallback = void(*)(int button, int state, int x, int y);
	using MouseMotionCallback = void(*)(int x, int y);

	class Window
	{
	public:
		void Init(const char* title, int* argcp, char** argv);
		void Create(WindowCallback init, WindowCallback render, bool use_msaa = false);
		void SetDestroyFunc(WindowCallback destory);
		void SetReshapeFunc(ReshapeCallback reshape) const;
		void SetKeyboardFunc(KeyboardCallback key_event) const;
		void SetKeyboardUpFunc(KeyboardCallback key_event) const;
		void SetMouseFunc(MouseCallback mouse_event) const;
		void SetMouseMotionFunc(MouseMotionCallback mouse_event) const;
		void SetMouseDragFunc(MouseMotionCallback mouse_event) const;
		void SetMouseWheelFunc(MouseCallback mouse_event) const;
		void SetIdleFunc(UpdateCallback idle);
		float GetPartialTime() const;
		int GetFps() const;
		void Loop() const;

	private:
		static void RenderWrapper();
		static void Update();

	private:
		static Window* inst_;
		WindowCallback destroy_ = nullptr;
		WindowCallback render_ = nullptr;
		UpdateCallback idle_ = nullptr;
		const char* title_ = "";
		float partial_time_ = 0;
		long last_frame_ = 0;
		long last_fps_count_frame_ = 0;
		int fps_count_ = 0;
		int fps_ = 0;
	};
}
