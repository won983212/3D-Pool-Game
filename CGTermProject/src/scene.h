#pragma once

#include <irrklang/irrKlang.h>
#include "gfx/gfx.h"
#include "gfx/vbo.h"
#include "gfx/camera.h"
#include "gfx/skybox.h"
#include "guiscreen.h"
#include "model/assetmodel.h"
#include "model/ball.h"
#include "pooltable.h"
#include "game.h"
#include "balltracer.h"
#include "util/util.h"
#include "gameenum.h"


struct LightData
{
	glm::vec4 position;
	glm::vec4 color;
};

struct ViewMatrixData
{
	glm::mat4 view;
	glm::mat4 projection;
};

struct MouseRay
{
	glm::vec3 position;
	glm::vec3 direction;
};

class CueTransform
{
public:
	glm::mat4 GetModelMatrix() const { return model_mat_; }
	glm::vec2 GetCueDirection() const { return glm::vec2(sin(rotation_), cos(rotation_)); }

	void Update()
	{
		model_mat_ = glm::mat4(1.0f);
		model_mat_ = translate(model_mat_, glm::vec3(position_.x, 0.16f, position_.y)); // location
		model_mat_ = rotate(model_mat_, rotation_, glm::vec3(0, 1, 0)); // rotation
		model_mat_ = translate(model_mat_, glm::vec3(0.0f, 0.0f, push_amount_)); // Push model
		model_mat_ = rotate(model_mat_, DEGTORAD(90.0f), glm::vec3(1, 0, 0));
		model_mat_ = scale(model_mat_, glm::vec3(0.05f));
	}

public:
	glm::vec2 position_ = glm::vec2(0);
	float rotation_ = 0;
	float push_amount_ = 0;
	CueMode mode_ = CueMode::Invisible;

private:
	glm::mat4 model_mat_ = glm::mat4(1.0f);
};

class Scene : public IBallEvent, IScreenChangedEvent
{
public:
	Scene() : 
		ubo_light_(GL_UNIFORM_BUFFER), ubo_view_(GL_UNIFORM_BUFFER), ball_tracer_(table_), 
		cam_(0, 30, 10), game(this, &ui_) {}
	void Init();
	void Update(float partial_time, int fps);
	void Render();
	void Mouse(int button, int state, int x, int y);
	void MouseWheel(int button, int state, int x, int y);
	void MouseMove(int x, int y);
	void MouseDrag(int button, int x, int y, int dx, int dy);
	void Keyboard(unsigned char key, int x, int y);
	virtual void OnScreenChanged(int id);
	virtual void OnAllBallStopped();
	virtual void OnBallHoleIn(int ball_id);
	virtual void OnWhiteBallCollide(int ball_id);
	void EnableCueControl();
	void DisableCueControl();
	void ResetWhiteBall();

private:
	void UpdateLight() const;
	void UpdateView() const;
	MouseRay CalculateMouseRay(int mouse_x, int mouse_y) const;
	void StrikeWhiteBall() const;

private:
	// camera ball view
	bool is_ball_view_ = false;
	// light
	LightData lights_[3];
	commoncg::VBO ubo_light_;
	// view
	ViewMatrixData view_;
	commoncg::VBO ubo_view_;
	// pool table physics simulator
	PoolTable table_;
	// ball ghost/line tracer
	BallTracer ball_tracer_;
	// game manager
	Game game;
	// models
	CueTransform cue_transform_;
	model::AssetModel model_pool_table_;
	model::AssetModel model_cue_;
	model::Ball model_ball_;
	// graphics
	commoncg::ShaderProgram shader_;
	commoncg::Skybox skybox_;
	commoncg::Camera cam_;
	commoncg::Texture brdf_lut_;
	commoncg::Texture* ball_textures_[BallCount];
	GuiScreen ui_;
};
