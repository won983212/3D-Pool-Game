#pragma once

class UIScreen
{
public:
	void init();
	void render();
	void mouse(int button, int state, int x, int y);
	void mouseDrag(int x, int y);
};