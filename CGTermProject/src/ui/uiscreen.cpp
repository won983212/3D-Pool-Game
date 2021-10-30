#include <iostream>
#include <vector>
#include "uiscreen.h"
#include "../util/util.h"

using namespace commoncg;

bool font_ready = false;
FontRenderer font_renderer;


FontRenderer* UIScreen::GetFontRenderer()
{
	if (!font_ready)
		font_renderer.Init("res/font.ttf");
	return &font_renderer;
}

UIScreen::~UIScreen()
{
	for (auto& element : elements_)
		for (auto& j : element)
			delete j;
}

void UIScreen::Init()
{
	Clear();
	ShaderProgram::Push();
	ui_shader_.AddShader("res/shader/gui.vert", GL_VERTEX_SHADER);
	ui_shader_.AddShader("res/shader/gui.frag", GL_FRAGMENT_SHADER);
	ui_shader_.Load();
	ui_shader_.Use();
	ui_shader_.SetUniform("projection", glm::ortho(0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 0.0f, 0.0f, 10.0f));
	ui_shader_.SetUniform("texture_diffuse", PBR_TEXTURE_INDEX_ALBEDO);
	ui_shader_.SetUniform("useTexture", false);
	ui_shader_.SetUniform("useFont", false);
	ShaderProgram::Pop();

	vao_.Create();
	vbo_.Create();
	ScreenInit();
}

void UIScreen::SetScreenChangedEvent(IScreenChangedEvent* e)
{
	screen_change_event_ = e;
}

void UIScreen::Add(UIElement* element, int screen_idx)
{
	element->parent_ = this;
	elements_[screen_idx].push_back(element);
}

void UIScreen::AddPages(int count)
{
	for (int i = 0; i < count; i++)
		elements_.emplace_back();
}

void UIScreen::SetScreen(int index)
{
	if (index < 0 || index >= elements_.size())
	{
		std::cout << "Warning: Invaild index: " << index << std::endl;
		return;
	}

	this->index_ = index;
	if (screen_change_event_ != nullptr)
		screen_change_event_->OnScreenChanged(index);
}

int UIScreen::GetCurrentScreen() const
{
	return index_;
}

void UIScreen::Clear()
{
	elements_.clear();
}

void UIScreen::Render()
{
	glDisable(GL_DEPTH_TEST);

	ShaderProgram::Push();
	ui_shader_.Use();
	glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_ALBEDO);

	vao_.Use();
	vbo_.Use();
	vao_.Attrib(0, 4, GL_FLOAT, sizeof(UIVertex), 0);
	vao_.Attrib(1, 4, GL_FLOAT, sizeof(UIVertex), offsetof(UIVertex, color));

	std::vector<UIVertex> vertices;
	for (const auto element : elements_[index_])
	{
		if (!element->visible_)
			continue;

		vertices.clear();
		ui_shader_.SetUniform("useTexture", element->Render(vertices));
		if (!vertices.empty())
		{
			vao_.Use();
			vbo_.Buffer(vertices.size() * sizeof(UIVertex), &vertices[0]);
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}
		element->PostRender();
	}
	OnRenderTick();

	vbo_.Unbind();
	VAO::Unbind();
	ShaderProgram::Pop();
	glEnable(GL_DEPTH_TEST);
}

void UIScreen::Mouse(int button, int state, int x, int y)
{
	for (const auto element : elements_[index_])
		if (element->OnMouse(button, state, x, y)) break;
}

void UIScreen::MouseDrag(int x, int y)
{
	for (const auto element : elements_[index_])
		element->OnMouseDrag(x, y);
}

void UIScreen::MouseWheel(int button, int state, int x, int y)
{
	for (const auto element : elements_[index_])
		element->OnMouseWheel(button, state, x, y);
}

void UIScreen::MouseMove(int x, int y)
{
	for (const auto element : elements_[index_])
		element->OnMouseMove(x, y);
}
