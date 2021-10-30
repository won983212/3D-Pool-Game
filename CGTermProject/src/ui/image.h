#pragma once
#include "../gfx/texture.h"
#include "uielement.h"

class UEImage : public UIElement
{
public:
	~UEImage();
	bool Render(std::vector<UIVertex>& vertices) override;
	void SetImage(const char* image_path);
	void SetUV(int x, int y, int w, int h);
	void PackSize();

private:
	commoncg::Texture* texture_ = nullptr;
	float u1_ = 0, v1_ = 0, u2_ = 1, v2_ = 1;
};
