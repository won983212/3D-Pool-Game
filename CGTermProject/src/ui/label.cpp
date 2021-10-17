#include "uiscreen.h"
#include "label.h"
#include "font.h"


bool UELabel::render(std::vector<UIVertex>& vertices)
{
	FontRenderer* fontRenderer = UIScreen::getFontRenderer();
	if(shadowColor != 0)
		fontRenderer->renderText(text, x + 1, y + 1, shadowColor, fontPoint, centered);
	fontRenderer->renderText(text, x, y, color, fontPoint, centered);
	return false;
}

void UELabel::setText(std::wstring text)
{
	this->text = text;
}

void UELabel::setColor(int color)
{
	this->color = color;
}

void UELabel::setShadowColor(int shadowColor)
{
	this->shadowColor = shadowColor;
}

void UELabel::setUseCentered(bool centered)
{
	this->centered = centered;
}

void UELabel::setTextPoint(float point)
{
	this->fontPoint = point;
}