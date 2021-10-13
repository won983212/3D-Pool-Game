#pragma once
#include "../gfx/gfx.h"
#include <vector>

typedef struct UIVertex
{
	glm::vec2 position;
	glm::vec2 texCoord;
	glm::vec4 color;
} UIVertex;

class UIScreen;
class UIElement
{
public:
	// return: use texture?
	virtual bool render(std::vector<UIVertex>& vertices) = 0;
	virtual void postRender() {};
	virtual void onMouse(int button, int state, int x, int y) {};
	virtual void onMouseWheel(int button, int state, int x, int y) {};
	virtual void onMouseDrag(int x, int y) {};
	virtual void onMouseMove(int x, int y) {};

public:
	void setBounds(float x, float y, float width, float height);
	void setLocation(float x, float y);
	void setSize(float width, float height);
	static UIVertex makeVertex(float x, float y, int color, float u, float v);
	static UIVertex makeVertex(float x, float y, int color);
	static UIVertex makeVertex(float x, float y, float u, float v);

public:
	int id = -1;
	float x;
	float y;
	float width;
	float height;
	UIScreen* parent;
};