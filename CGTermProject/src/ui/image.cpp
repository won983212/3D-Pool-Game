#include "image.h"

UEImage::~UEImage()
{
	delete texture_;
}

bool UEImage::Render(std::vector<UIVertex>& vertices)
{
	if (texture_ == nullptr)
		return false;

	vertices.push_back(MakeVertex(x_, y_, u1_, v1_));
	vertices.push_back(MakeVertex(x_ + width_, y_, u2_, v1_));
	vertices.push_back(MakeVertex(x_, y_ + height_, u1_, v2_));
	vertices.push_back(MakeVertex(x_ + width_, y_, u2_, v1_));
	vertices.push_back(MakeVertex(x_ + width_, y_ + height_, u2_, v2_));
	vertices.push_back(MakeVertex(x_, y_ + height_, u1_, v2_));

	texture_->Bind();
	return true;
}

void UEImage::SetImage(const char* image_path)
{
	delete texture_;
	texture_ = commoncg::Texture::CacheImage(image_path);
}

void UEImage::SetUV(int x, int y, int w, int h)
{
	u1_ = static_cast<float>(x) / texture_->GetWidth();
	v1_ = static_cast<float>(y) / texture_->GetHeight();
	u2_ = static_cast<float>(x + w) / texture_->GetWidth();
	v2_ = static_cast<float>(y + h) / texture_->GetHeight();
}

void UEImage::PackSize()
{
	if (texture_ == nullptr)
		return;
	width_ = static_cast<float>(texture_->GetWidth());
	height_ = static_cast<float>(texture_->GetHeight());
}
