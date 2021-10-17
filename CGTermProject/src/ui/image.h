#pragma once
#include "../gfx/texture.h"
#include "uielement.h"

class UEImage : public UIElement
{
public:
	~UEImage();
	virtual bool render(std::vector<UIVertex>& vertices);
	void setImage(const char* imagePath);
	void setUV(int x, int y, int w, int h);
	void packSize();
private:
	commoncg::Texture* texture = nullptr;
	float u1 = 0, v1 = 0, u2 = 1, v2 = 1;
};