#pragma once
#include "../gfx/gfx.h"
#include <vector>

typedef struct UIVertex
{
	glm::vec2 position;
	glm::vec2 tex_coord;
	glm::vec4 color;
} UIVertex;

class UIScreen;

class UIElement
{
public:
	// return: Use texture?
	virtual bool Render(std::vector<UIVertex>& vertices) = 0;
	virtual void PostRender() { }
	virtual bool OnMouse(int button, int state, int x, int y) { return false; }
	virtual void OnMouseWheel(int button, int state, int x, int y) { }
	virtual void OnMouseDrag(int x, int y) { }
	virtual void OnMouseMove(int x, int y) { }

public:
	void SetBounds(float x, float y, float width, float height);
	void SetLocation(float x, float y);
	void SetSize(float width, float height);
	void SetVisible(bool visible);
	static UIVertex MakeVertex(float x, float y, unsigned int color, float u, float v);
	static UIVertex MakeVertex(float x, float y, unsigned int color);
	static UIVertex MakeVertex(float x, float y, float u, float v);

public:
	int id_ = -1;
	float x_;
	float y_;
	float width_;
	float height_;
	bool visible_ = true;
	UIScreen* parent_;
};
