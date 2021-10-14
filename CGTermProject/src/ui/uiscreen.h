#pragma once
#include <vector>
#include "uielement.h"
#include "font.h"
#include "../gfx/vao.h"
#include "../gfx/shader.h"

class UIScreen
{
public:
	~UIScreen();
	void init();
	void render();
	void mouse(int button, int state, int x, int y);
	void mouseWheel(int button, int state, int x, int y);
	void mouseDrag(int x, int y);
	void mouseMove(int x, int y);
	void setScreen(int index);
	int getCurrentScreen();
	virtual void screenInit() {};
	virtual void onRenderTick() {};
	static FontRenderer* getFontRenderer();
protected:
	void add(UIElement* element, int screenIdx);
	void addPages(int count = 1);
	void clear();
private:
	int index = 0;
	commoncg::ShaderProgram uiShader;
	commoncg::VAO vao;
	commoncg::VBO vbo;
	std::vector<std::vector<UIElement*>> elements;
};