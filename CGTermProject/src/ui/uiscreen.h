#pragma once
#include "../gfx/vao.h"
#include "../gfx/shader.h"

typedef struct UIVertex
{
	glm::vec2 position;
	glm::vec4 color;
	glm::vec2 texCoord;
} UIVertex;

class UIScreen
{
public:
	void init();
	void render(glm::mat4 view);
	void mouse(int button, int state, int x, int y);
	void mouseDrag(int x, int y);
private:
	commoncg::ShaderProgram uiShader;
	commoncg::VAO vao;
	commoncg::VBO vbo;
};