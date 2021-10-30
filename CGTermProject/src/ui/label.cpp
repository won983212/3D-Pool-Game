#include "uiscreen.h"
#include "label.h"
#include "font.h"


bool UELabel::Render(std::vector<UIVertex>& vertices)
{
	FontRenderer* font_renderer = UIScreen::GetFontRenderer();
	if (shadow_color_ != 0)
		font_renderer->RenderText(text_, x_ + 1, y_ + 1, shadow_color_, font_point_, centered_);
	font_renderer->RenderText(text_, x_, y_, color_, font_point_, centered_);
	return false;
}

void UELabel::SetText(std::wstring text)
{
	this->text_ = std::move(text);
}

void UELabel::SetColor(unsigned int color)
{
	this->color_ = color;
}

void UELabel::SetShadowColor(unsigned int shadow_color)
{
	this->shadow_color_ = shadow_color;
}

void UELabel::SetUseCentered(bool centered)
{
	this->centered_ = centered;
}

void UELabel::SetTextPoint(float point)
{
	this->font_point_ = point;
}
