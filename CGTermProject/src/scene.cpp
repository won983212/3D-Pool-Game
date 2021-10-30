#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "scene.h"
#include "gfx/texture.h"
#include "model/quad.h"
#include "ui/button.h"
#include "util/sound.h"
#include "strings.h"

using namespace glm;
using namespace commoncg;

// cue constants
constexpr float MinCuePower = 0.05f;
constexpr float MaxCuePower = 1.0f;
constexpr float CuePowerModifier = 12.0f;

// projection constants
constexpr float PrjFov = DEGTORAD(45.0f);
constexpr float PrjAspect = static_cast<float>(SCREEN_WIDTH) / SCREEN_HEIGHT;
constexpr float PrjNear = 0.1f;
constexpr float PrjFar = 100.0f;

constexpr model::Material BallMaterial =
{
	vec4(1.0f, 1.0f, 0.0f, 1.0f), // albedo
	0.1f, // metallic
	0.1f, // roughness
	DefaultAo, // ao
	{0, -1, -1, -1} // texIndex
};


void Scene::Init()
{
	// initalize graphic variables
	brdf_lut_.LoadTextureImage("res/texture/brdf.png");
	ubo_light_.Create();
	ubo_view_.Create();

	// prepare ball textures
	for (int i = 0; i < BallCount; i++)
		ball_textures_[i] = Texture::CacheImage(("res/texture/ball/ball_" + std::to_string(i) + ".png").c_str());

	// model
	model_pool_table_.LoadModel("res/model/pooltable.gltf");
	model_cue_.LoadModel("res/model/cue.gltf");
	model_ball_.Init(BallRadius);

	// shader
	shader_.AddShader("res/shader/pbr.vert", GL_VERTEX_SHADER);
	shader_.AddShader("res/shader/pbr.frag", GL_FRAGMENT_SHADER);
	shader_.Load();
	shader_.Use();
	shader_.SetUniform("highlightColor", vec4(0.0f));
	shader_.SetUniform("texture_albedo", PBR_TEXTURE_INDEX_ALBEDO);
	shader_.SetUniform("texture_metallic", PBR_TEXTURE_INDEX_METALLIC);
	shader_.SetUniform("texture_roughness", PBR_TEXTURE_INDEX_ROUGHNESS);
	shader_.SetUniform("texture_normal", PBR_TEXTURE_INDEX_NORMAL);
	shader_.SetUniform("irradianceMap", PBR_TEXTURE_INDEX_IRRADIANCEMAP);
	shader_.SetUniform("specularMap", PBR_TEXTURE_INDEX_SPECULARMAP);
	shader_.SetUniform("brdfMap", PBR_TEXTURE_INDEX_BRDFMAP);

	// gui screen
	ui_.Init();

	// events setup
	ui_.SetScreenChangedEvent(this);
	table_.SetBallEvent(this);
	ball_tracer_.Init();

	// skybox
	skybox_.BeginLoad();
	skybox_.LoadHDRSkybox("res/texture/skybox/skybox.hdr");
	skybox_.LoadDDSIrradianceMap("res/texture/skybox/irr.dds");
	skybox_.LoadDDSSpecularMap("res/texture/skybox/env.dds");
	skybox_.EndLoad();

	// lights
	LightData light;
	light.position = vec4(0.0f, 3.0f, 5.0f, 1.0f);
	light.color = vec4(50);
	lights_[0] = light;

	light.position = vec4(0.0f, 3.0f, 0.0f, 1.0f);
	light.color = vec4(50);
	lights_[1] = light;

	light.position = vec4(0.0f, 3.0f, -5.0f, 1.0f);
	light.color = vec4(50);
	lights_[2] = light;
	UpdateLight();

	// camera (view)
	cam_.GetViewMatrix(&view_.view);
	view_.projection = perspective(PrjFov, PrjAspect, PrjNear, PrjFar);
	UpdateView();
}

void Scene::Update(float partial_time, int fps)
{
	if (is_ball_view_)
	{
		vec2 pos = table_.GetBalls()[0]->position_;
		if (pos.x != cam_.center_.x || pos.y != cam_.center_.z)
		{
			cam_.center_ = vec3(pos.x, BallRadius / 2, pos.y);
			cam_.Update();
		}
	}
	else if (cam_.center_ != vec3(0.0f))
	{
		cam_.center_ = vec3(0.0f);
		cam_.Update();
	}

	if (!ball_placing_ && ui_.GetCurrentScreen() == 2)
		table_.Update(partial_time);
	ui_.fps_label_->SetText(std::wstring(L"FPS: ") + std::to_wstring(fps));
}

void Scene::UpdateLight() const
{
	ubo_light_.BindBufferRange(UNIFORM_BINDING_LIGHT, 0, sizeof lights_);
	ubo_light_.Buffer(sizeof lights_, &lights_);
	ubo_light_.Unbind();
}

void Scene::UpdateView() const
{
	ubo_view_.BindBufferRange(UNIFORM_BINDING_VIEWMAT, 0, sizeof view_);
	ubo_view_.Buffer(sizeof view_, &view_);
	ubo_view_.Unbind();
}

void Scene::ResetGame()
{
	turn_ = true;
	ball_goals_[0] = 0;
	ball_goals_[1] = 0;
	my_ball_count_ = 0;
	first_touch_ball_ = 0;
	group_ = BallGroup::NotDecided;
	is_first_group_set_ = false;
	is_foul_ = false;
	is_turn_out_ = false;
	ball_placing_ = false;
}

void Scene::Render()
{
	// Update only when cam has changed.
	if (cam_.GetViewMatrix(&view_.view))
		UpdateView();
	shader_.SetUniform("camPos", cam_.GetEyePosition());

	// Bind env maps and brdf LUT (lookup texture).
	skybox_.BindEnvironmentTextures();
	glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_BRDFMAP);
	brdf_lut_.Bind();

	// balls
	BindMaterial(&BallMaterial);
	const std::vector<Ball*> balls = table_.GetBalls();
	for (unsigned int i = 0; i < balls.size(); i++)
	{
		if (!balls[i]->visible_)
			continue;

		if (balls[i]->highlight_)
			shader_.SetUniform("highlightColor", vec4(1.0f, 0.0f, 0.0f, 1.0f));

		vec2 pos = balls[i]->position_;
		auto model = mat4(1.0f);

		model = translate(model, vec3(pos.x, BallRadius, pos.y));
		model = model * toMat4(balls[i]->rotation_);

		glActiveTexture(GL_TEXTURE0 + PBR_TEXTURE_INDEX_ALBEDO);
		ball_textures_[i]->Bind();
		shader_.SetUniform("model", model);
		model_ball_.Draw();

		if (balls[i]->highlight_)
			shader_.SetUniform("highlightColor", vec4(0.0f));
	}
	Texture::Unbind();

	// Render pool table
	auto model = mat4(1.0f);
	model = scale(model, vec3(5.0f));
	model = rotate(model, DEGTORAD(90.0f), vec3(1, 0, 0));
	shader_.SetUniform("model", model);
	model_pool_table_.Draw();

	// Render cue
	if (cue_transform_.mode_ != CueMode::Invisible)
	{
		const vec2 white_ball_pos = table_.GetBalls()[0]->position_;
		if (cue_transform_.position_ != white_ball_pos)
		{
			cue_transform_.position_ = white_ball_pos;
			cue_transform_.Update();
		}
		shader_.SetUniform("model", cue_transform_.GetModelMatrix());
		model_cue_.Draw();
	}

	// ball tracer
	if (cue_transform_.mode_ == CueMode::Rotation)
		ball_tracer_.Draw();

	// skybox
	skybox_.Render(view_.view);

	// gui
	ui_.Render();
}

MouseRay Scene::CalculateMouseRay(int mouse_x, int mouse_y) const
{
	MouseRay ray;

	float sx = mouse_x * 2.0f / SCREEN_WIDTH - 1.0f; // to -1 ~ 1
	float sy = mouse_y * 2.0f / SCREEN_HEIGHT - 1.0f; // to -1 ~ 1
	const float half_height = tan(PrjFov / 2) * PrjNear; // near rectangle height/2

	// fits to near rectangle
	sx *= PrjAspect * half_height;
	sy *= half_height;

	ray.position = vec3(cam_.GetEyePosition());
	ray.direction = -cam_.GetUp() * sy + cam_.GetRight() * sx;

	ray.position = ray.position + ray.direction;
	ray.direction += normalize(cam_.GetFront()) * PrjNear;
	ray.direction = normalize(ray.direction);

	return ray;
}

void Scene::Keyboard(unsigned char key, int x, int y)
{
	if (key == ' ' && !is_foul_ && !ball_placing_)
	{
		is_ball_view_ = !is_ball_view_;
		ui_.ShowMessage(is_ball_view_ ? MSG_VIEW_CENTER_BALL : MSG_VIEW_CENTER_TABLE);
	}
}

void Scene::Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON && ui_.GetCurrentScreen() == 2)
	{
		if (ball_placing_)
		{
			if (!table_.GetBalls()[0]->highlight_)
			{
				ball_placing_ = false;
				EnableCueControl();
				ui_.ShowMessage(MSG_TURN(turn_));
			}
			else
			{
				ui_.ShowMessage(MSG_CANNOT_PLACE_THERE);
			}
		}
		else if (cue_transform_.mode_ == CueMode::Rotation)
		{
			cue_transform_.mode_ = CueMode::Pushing;
		}
		else if (cue_transform_.mode_ == CueMode::Pushing)
		{
			HitWhiteBall();
			cue_transform_.mode_ = CueMode::Invisible;
			cue_transform_.push_amount_ = -BallRadius;
			cue_transform_.Update();
		}
	}

	ui_.Mouse(button, state, x, y);
}

void Scene::MouseDrag(int button, int x, int y, int dx, int dy)
{
	ui_.MouseDrag(x, y);
	if (ui_.GetCurrentScreen() != 2)
		return;

	if (button == GLUT_RIGHT_BUTTON)
	{
		cam_.yaw_ += dx / 8.0f;
		cam_.pitch_ += dy / 8.0f;
		cam_.Update();
	}
}

void Scene::MouseWheel(int button, int state, int x, int y)
{
	ui_.MouseWheel(button, state, x, y);
	if (ui_.GetCurrentScreen() != 2)
		return;

	if (state > 0)
		cam_.zoom_ -= 0.5f;
	else
		cam_.zoom_ += 0.5f;
	cam_.Update();
}

void Scene::MouseMove(int x, int y)
{
	ui_.MouseMove(x, y);
	if (ui_.GetCurrentScreen() != 2)
		return;

	MouseRay ray = CalculateMouseRay(x, y);
	Ball* ball = table_.GetBalls()[0];

	if (ball_placing_)
	{
		vec3 hit = ray.position - ray.direction * (ray.position.y / ray.direction.y);
		ball->position_ = vec2(hit.x, hit.z);
		ball->highlight_ = !table_.CanPlaceWhiteBall();
		return;
	}

	if (cue_transform_.mode_ == CueMode::Invisible)
		return;

	// 바닥 면(y=yLevel=0.16f)과 intersect되는 점을 calculate
	// pos + dir * t = 0.16f(y) 인 t를 찾으면 된다.
	constexpr float y_level = 0.16f;
	float t = (y_level - ray.position.y) / ray.direction.y;
	vec3 hit = ray.position + ray.direction * t;
	vec2 diff = cue_transform_.position_ - vec2(hit.x, hit.z);

	if (cue_transform_.mode_ == CueMode::Rotation)
	{
		float angle;
		if (length2(diff) == 0.0f)
		{
			angle = 0;
		}
		else
		{
			angle = atan2(diff.y, diff.x);
			ball_tracer_.position_ = ball->position_;
			ball_tracer_.direction_ = normalize(diff);
			ball_tracer_.Update();
		}
		cue_transform_.rotation_ = -angle + DEGTORAD(90.0f);
	}
	else if (cue_transform_.mode_ == CueMode::Pushing)
	{
		const float power = dot(cue_transform_.GetCueDirection(), diff);
		cue_transform_.push_amount_ = -clamp(power, BallRadius + MinCuePower, BallRadius + MaxCuePower);
	}

	cue_transform_.Update();
}

void Scene::OnScreenChanged(int id)
{
	if (id == 2)
	{
		ResetGame();
		SetTurn(true);
		EnableCueControl();
		table_.ResetBallPosition();
	}
}

void Scene::OnAllBallStopped()
{
	if (ui_.GetCurrentScreen() != 2 || ball_placing_)
		return;

	// set foul when first touch is not my ball or none;
	const bool is_solid = first_touch_ball_ >= 1 && first_touch_ball_ <= 7;
	if (first_touch_ball_ == 0 || group_ != BallGroup::NotDecided && !is_first_group_set_ && is_solid == (group_ == BallGroup::P1Solid) != turn_)
	{
		is_foul_ = true;
		is_turn_out_ = true;
	}

	// turn change
	if (is_turn_out_ || !is_first_group_set_ && my_ball_count_ == 0)
	{
		SetTurn(!turn_);
		is_turn_out_ = false;
	}

	my_ball_count_ = 0;
	is_first_group_set_ = false;
	first_touch_ball_ = 0;

	// foul
	if (is_foul_)
	{
		ProcessFoul();
		is_foul_ = false;
	}
	else
	{
		EnableCueControl();
	}
}

void Scene::OnBallHoleIn(int ball_id)
{
	// TODO ball goal-in sound
	GetSoundEngine()->play2D(SOUND_CUE_PUSH(1));

	// foul. white ball is in hole.
	if (ball_id == 0)
	{
		is_ball_view_ = false;
		is_foul_ = true;
	}

	if (ball_id == 8)
	{
		const bool win = ball_goals_[turn_ == (group_ == BallGroup::P1Strip)] == 7;
		ui_.GoGameEnd(MSG_WIN(turn_));
		return;
	}

	const bool is_solid = ball_id >= 1 && ball_id <= 7;
	if (group_ == BallGroup::NotDecided)
	{
		group_ = turn_ == is_solid ? BallGroup::P1Solid : BallGroup::P1Strip;
		ui_.ShowMessage(MSG_DECIDED_BALL(turn_, is_solid));
		is_first_group_set_ = true;
	}
	else
	{
		if (is_solid == (group_ == BallGroup::P1Solid) != turn_)
		{
			is_turn_out_ = true;
		}
		else
		{
			my_ball_count_++;
		}
	}

	ball_goals_[!is_solid]++;
	SetTurn(turn_); // Update progress
}

void Scene::OnWhiteBallCollide(int ball_id)
{
	if (first_touch_ball_ == 0)
	{
		first_touch_ball_ = ball_id;
	}
}

void Scene::SetTurn(bool turn)
{
	// get percent of ball goal count
	float percent = 0.0f;
	if (group_ != BallGroup::NotDecided)
		percent = ball_goals_[turn == (group_ == BallGroup::P1Strip)] / 8.0f;

	ui_.SetTurn(turn, group_, percent);
	if (this->turn_ != turn)
	{
		ui_.ShowMessage(MSG_TURN(turn));
		this->turn_ = turn;
	}
}

void Scene::EnableCueControl()
{
	cue_transform_.mode_ = CueMode::Rotation;
	cue_transform_.push_amount_ = -BallRadius;
	cue_transform_.Update();
	ball_tracer_.position_ = table_.GetBalls()[0]->position_;
	ball_tracer_.Update();
}

void Scene::HitWhiteBall() const
{
	Ball* ball = table_.GetBalls()[0];
	const float power = -(cue_transform_.push_amount_ + BallRadius);
	const int power_level = power > MaxCuePower * 0.5f;

	GetSoundEngine()->play2D(SOUND_CUE_PUSH(power_level));
	ball->velocity_ = power * CuePowerModifier * cue_transform_.GetCueDirection();
}

void Scene::ProcessFoul()
{
	ui_.ShowMessage(MSG_FOUL);
	table_.GetBalls()[0]->velocity_ = vec2(0.0f);
	table_.GetBalls()[0]->position_ = vec2(0, -TableHeight / 3);
	table_.GetBalls()[0]->visible_ = true;
	cue_transform_.mode_ = CueMode::Invisible;
	ball_placing_ = true;
}
