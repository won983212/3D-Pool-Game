#include <iostream>
#include <vector>
#include "uiscreen.h"
#include "../util/util.h"

using namespace commoncg;

bool fontReady = false;
FontRenderer fontRenderer;


FontRenderer* UIScreen::getFontRenderer()
{
	if (!fontReady)
		fontRenderer.init("res/font.ttf");
	return &fontRenderer;
}

UIScreen::~UIScreen()
{
	for (unsigned int i = 0; i < elements.size(); i++)
		for (unsigned int j = 0; j < elements[i].size(); j++)
			delete elements[i][j];
}

void UIScreen::init()
{
	clear();
	ShaderProgram::push();
	uiShader.addShader("res/shader/gui.vert", GL_VERTEX_SHADER);
	uiShader.addShader("res/shader/gui.frag", GL_FRAGMENT_SHADER);
	uiShader.load();
	uiShader.use();
	uiShader.setUniform("projection", glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, 0.0f, 10.0f));
	uiShader.setUniform("texture_diffuse", PBR_TEXTURE_INDEX_ALBEDO);
	uiShader.setUniform("useTexture", false);
	uiShader.setUniform("useFont", false);
	ShaderProgram::pop();

	vao.create();
	vbo.create();
	screenInit();
}

void UIScreen::add(UIElement* element, int screenIdx)
{
	element->parent = this;
	elements[screenIdx].push_back(element);
}

void UIScreen::addPages(int count)
{
	for (int i = 0; i < count; i++)
		elements.push_back(std::vector<UIElement*>());
}

void UIScreen::setScreen(int index)
{
	if (index < 0 || index >= elements.size())
	{
		std::cout << "Warning: Invaild index: " << index << std::endl;
		return;
	}

	this->index = index;
}

int UIScreen::getCurrentScreen()
{
	return index;
}

void UIScreen::clear()
{
	elements.clear();
}

void UIScreen::render()
{
	glDisable(GL_DEPTH_TEST);

	ShaderProgram::push();
	uiShader.use();
	glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_ALBEDO);

	vao.use();
	vbo.use();
	vao.attr(0, 4, GL_FLOAT, sizeof(UIVertex), 0);
	vao.attr(1, 4, GL_FLOAT, sizeof(UIVertex), offsetof(UIVertex, color));

	std::vector<UIVertex> vertices;
	for (unsigned int i = 0; i < elements[index].size(); i++)
	{
		UIElement* element = elements[index][i];
		if (!element->visible)
			continue;

		vertices.clear();
		uiShader.setUniform("useTexture", element->render(vertices));
		if (!vertices.empty())
		{
			vao.use();
			vbo.buffer(vertices.size() * sizeof(UIVertex), &vertices[0]);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}
		element->postRender();
	}
	onRenderTick();

	vbo.unbind();
	VAO::unbind();
	ShaderProgram::pop();
	glEnable(GL_DEPTH_TEST);
}

void UIScreen::mouse(int button, int state, int x, int y)
{
	for (unsigned int i = 0; i < elements[index].size(); i++)
		if (elements[index][i]->onMouse(button, state, x, y)) break;
}

void UIScreen::mouseDrag(int x, int y)
{
	for (unsigned int i = 0; i < elements[index].size(); i++)
		elements[index][i]->onMouseDrag(x, y);
}

void UIScreen::mouseWheel(int button, int state, int x, int y)
{
	for (unsigned int i = 0; i < elements[index].size(); i++)
		elements[index][i]->onMouseWheel(button, state, x, y);
}

void UIScreen::mouseMove(int x, int y)
{
	for (unsigned int i = 0; i < elements[index].size(); i++)
		elements[index][i]->onMouseMove(x, y);
}