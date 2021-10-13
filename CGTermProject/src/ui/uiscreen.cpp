#include <vector>
#include "uiscreen.h"
#include "../util/util.h"

using namespace commoncg;

void UIScreen::init()
{
	ShaderProgram::push();
	uiShader.addShader("res/shader/gui.vert", GL_VERTEX_SHADER);
	uiShader.addShader("res/shader/gui.frag", GL_FRAGMENT_SHADER);
	uiShader.load();
	uiShader.use();
	uiShader.setUniform("projection", glm::ortho(0.0f, (float)SCREEN_WIDTH, (float)SCREEN_HEIGHT, 0.0f, 0.0f, 10.0f));
	uiShader.setUniform("texture_diffuse", PBR_TEXTURE_INDEX_ALBEDO);
	ShaderProgram::pop();

	vao.create();
	vbo.create();

	std::vector<UIVertex> vertices;
	vertices.push_back({ {50, 50}, {1, 1, 1, 1}, {0, 0} });
	vertices.push_back({ {300, 50}, {1, 1, 1, 1}, {1, 0} });
	vertices.push_back({ {50, 300}, {1, 1, 1, 1}, {0, 1} });
	vertices.push_back({ {300, 50}, {1, 1, 1, 1}, {1, 0} });
	vertices.push_back({ {300, 300}, {1, 1, 1, 1}, {1, 1} });
	vertices.push_back({ {50, 300}, {1, 1, 1, 1}, {0, 1} });

	vao.use();
	vbo.buffer(vertices.size() * sizeof(UIVertex), &vertices[0]);
	vao.attr(0, 2, GL_FLOAT, sizeof(UIVertex), 0);
	vao.attr(1, 4, GL_FLOAT, sizeof(UIVertex), offsetof(UIVertex, color));
	vao.attr(2, 2, GL_FLOAT, sizeof(UIVertex), offsetof(UIVertex, texCoord));
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	vbo.unbind();
	VAO::unbind();
}

void UIScreen::render(glm::mat4 view)
{
	glDisable(GL_DEPTH_TEST);
	ShaderProgram::push();
	uiShader.use();
	glActiveTexture(PBR_TEXTURE_INDEX_ALBEDO);

	vao.use();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	VAO::unbind();
	ShaderProgram::pop();
	glEnable(GL_DEPTH_TEST);
}

void UIScreen::mouse(int button, int state, int x, int y)
{

}

void UIScreen::mouseDrag(int x, int y)
{

}