#include <iostream>
#include "uiscreen.h"
#include "button.h"

bool UEButton::render(std::vector<UIVertex>& vertices)
{
	const float x2 = x + width;
	const float y2 = y + height;
	const int color = hover ? 0xaa999999 : 0xaa000000;

	vertices.push_back(makeVertex(x, y, color));
	vertices.push_back(makeVertex(x2, y, color));
	vertices.push_back(makeVertex(x, y2, color));
	vertices.push_back(makeVertex(x2, y, color));
	vertices.push_back(makeVertex(x2, y2, color));
	vertices.push_back(makeVertex(x, y2, color));
	return false;
}

void UEButton::postRender()
{
	FontRenderer* fontRenderer = UIScreen::getFontRenderer();
	fontRenderer->renderText(text, x + width / 2.0f, y + height / 2.0f, 0xffffffff, (height * textScale) / FONT_HEIGHT, true);
}

void UEButton::onMouse(int button, int state, int x, int y)
{
	if (hover && parent && id != -1 && state == GLUT_DOWN)
		parent->onButtonClick(id);
}

void UEButton::onMouseMove(int x, int y)
{
	if (x > this->x && y > this->y)
	{
		if (x < this->x + this->width && y < this->y + this->height)
		{
			hover = true;
			return;
		}
	}
	hover = false;
}

void UEButton::setText(std::wstring text)
{
	this->text = text;
}

void UEButton::setTextScale(float scale)
{
	this->textScale = scale;
}