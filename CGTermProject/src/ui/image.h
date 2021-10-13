#pragma once
#include "../gfx/texture.h"
#include "uielement.h"

class Image : public UIElement
{
public:
	~Image();
	virtual bool render(std::vector<UIVertex>& vertices);
	void setImage(const char* imagePath);
	void packSize();
private:
	 commoncg::Texture* texture = nullptr;
};