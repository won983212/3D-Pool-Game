#pragma once
#include <vector>
#include "uielement.h"
#include "font.h"
#include "event.h"
#include "../gfx/vao.h"
#include "../gfx/shader.h"

class UIScreen
{
public:
	~UIScreen();
	void Init();
	void Render();
	void Mouse(int button, int state, int x, int y);
	void MouseWheel(int button, int state, int x, int y);
	void MouseDrag(int x, int y);
	void MouseMove(int x, int y);
	void SetScreen(int index);
	int GetCurrentScreen() const;
	void SetScreenChangedEvent(IScreenChangedEvent* e);
	virtual void ScreenInit() { }
	virtual void OnRenderTick() { }
	static FontRenderer* GetFontRenderer();

protected:
	void Add(UIElement* element, int screen_idx);
	void AddPages(int count = 1);
	void Clear();

private:
	int index_ = 0;
	commoncg::ShaderProgram ui_shader_;
	commoncg::VAO vao_;
	commoncg::VBO vbo_;
	std::vector<std::vector<UIElement*>> elements_;
	IScreenChangedEvent* screen_change_event_ = nullptr;
};
