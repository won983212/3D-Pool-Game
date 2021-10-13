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
	virtual void init();
	void render();
	void mouse(int button, int state, int x, int y);
	void mouseWheel(int button, int state, int x, int y);
	void mouseDrag(int x, int y);
	void mouseMove(int x, int y);
	virtual void onButtonClick(int id) {};
	static FontRenderer* getFontRenderer();
protected:
	void add(UIElement* element);
	void clear();
private:
	commoncg::ShaderProgram uiShader;
	commoncg::VAO vao;
	commoncg::VBO vbo;
	std::vector<UIElement*> elements;
};