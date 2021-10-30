#include <iostream>
#include <utility>
#include "uiscreen.h"
#include "button.h"

bool UEButton::Render(std::vector<UIVertex>& vertices)
{
	const float x2 = x_ + width_;
	const float y2 = y_ + height_;
	const int color = hover_ ? 0xaa999999 : 0xaa000000;

	vertices.push_back(MakeVertex(x_, y_, color));
	vertices.push_back(MakeVertex(x2, y_, color));
	vertices.push_back(MakeVertex(x_, y2, color));
	vertices.push_back(MakeVertex(x2, y_, color));
	vertices.push_back(MakeVertex(x2, y2, color));
	vertices.push_back(MakeVertex(x_, y2, color));
	return false;
}

void UEButton::PostRender()
{
	FontRenderer* font_renderer = UIScreen::GetFontRenderer();
	font_renderer->RenderText(text_, x_ + width_ / 2.0f, y_ + height_ / 2.0f, 0xffffffff, height_ * text_scale_ / 0.75f, true);
}

bool UEButton::OnMouse(int button, int state, int x, int y)
{
	if (hover_ && parent_ && id_ != -1 && state == GLUT_DOWN)
	{
		if (this->btn_event_ != nullptr)
			this->btn_event_->OnButtonClick(id_);
		return true;
	}
	return false;
}

void UEButton::OnMouseMove(int x, int y)
{
	if (x > this->x_ && y > this->y_)
	{
		if (x < this->x_ + this->width_ && y < this->y_ + this->height_)
		{
			hover_ = true;
			return;
		}
	}
	hover_ = false;
}

void UEButton::SetText(std::wstring text)
{
	this->text_ = std::move(text);
}

void UEButton::SetTextScale(float scale)
{
	this->text_scale_ = scale;
}

void UEButton::SetButtonEvent(IButtonEvent* e)
{
	this->btn_event_ = e;
}
