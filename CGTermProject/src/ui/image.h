#pragma once
#include "../gfx/texture.h"
#include "uielement.h"

class UEImage : public UIElement
{
public:
	~UEImage();
	virtual bool render(std::vector<UIVertex>& vertices);
	void setImage(const char* imagePath);
	void packSize();
private:
	commoncg::Texture* texture = nullptr;
};