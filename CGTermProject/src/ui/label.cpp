#include "uiscreen.h"
#include "label.h"
#include "font.h"


bool Label::render(std::vector<UIVertex>& vertices)
{
	FontRenderer* fontRenderer = UIScreen::getFontRenderer();
	if(shadowColor != 0)
		fontRenderer->renderText(text, x + 1, y + 1, shadowColor, fontPoint, centered);
	fontRenderer->renderText(text, x, y, color, fontPoint, centered);
	return false;
}

void Label::setText(std::wstring text)
{
	this->text = text;
}

void Label::setColor(int color)
{
	this->color = color;
}

void Label::setShadowColor(int shadowColor)
{
	this->shadowColor = shadowColor;
}

void Label::setUseCentered(bool centered)
{
	this->centered = centered;
}

void Label::setTextPoint(float point)
{
	this->fontPoint = point;
}