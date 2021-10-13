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
		delete elements[i];
}

void UIScreen::init()
{
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
}

void UIScreen::add(UIElement* element)
{
	element->parent = this;
	elements.push_back(element);
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
	for (unsigned int i = 0; i < elements.size(); i++)
	{
		vertices.clear();
		uiShader.setUniform("useTexture", elements[i]->render(vertices));
		if (!vertices.empty())
		{
			vao.use();
			vbo.buffer(vertices.size() * sizeof(UIVertex), &vertices[0]);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}
		elements[i]->postRender();
	}

	vbo.unbind();
	VAO::unbind();
	ShaderProgram::pop();
	glEnable(GL_DEPTH_TEST);
}

void UIScreen::mouse(int button, int state, int x, int y)
{
	for (unsigned int i = 0; i < elements.size(); i++)
		elements[i]->onMouse(button, state, x, y);
}

void UIScreen::mouseDrag(int x, int y)
{
	for (unsigned int i = 0; i < elements.size(); i++)
		elements[i]->onMouseDrag(x, y);
}

void UIScreen::mouseWheel(int button, int state, int x, int y)
{
	for (unsigned int i = 0; i < elements.size(); i++)
		elements[i]->onMouseWheel(button, state, x, y);
}

void UIScreen::mouseMove(int x, int y)
{
	for (unsigned int i = 0; i < elements.size(); i++)
		elements[i]->onMouseMove(x, y);
}