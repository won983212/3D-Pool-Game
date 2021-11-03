#include "particle.h"
#include "shader.h"
#include "texture.h"
#include "../model/model.h"
#include "../util/util.h"

using namespace commoncg;

model::Material ParticleMaterialTemplate =
{
	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // albedo
	0.1f, // metallic
	0.8f, // roughness
	DefaultAo, // ao
	{-1, -1, -1, -1} // texIndex
};

// 0 ¡Â H ¡Â 360, 0 ¡Â S ¡Â 1, 0 ¡Â V ¡Â 1
static glm::vec3 HSVtoRGB(float H, float S, float V)
{
	float C = S * V;
	float X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	float m = V - C;
	float rgbMap[] = {C, X, 0, 0, X, C, X, C, C, X, 0, 0, 0, 0, X, C, C, X};
	int rgbIdx = (int)(H / 60.0f);
	
	glm::vec3 rgb;
	rgb.r = rgbMap[rgbIdx] + m;
	rgb.g = rgbMap[rgbIdx + 6] + m;
	rgb.b = rgbMap[rgbIdx + 12] + m;
	return rgb;
}

Particle::Particle() : position_(0.0f), velocity_(0.0f)
{
	color_ = HSVtoRGB(RandomFloat(0, 360), 1, 1);
	rotation_ = angleAxis(90.0f, glm::vec3(0, 1, 0));
	rotation_axis_ = glm::vec3(RandomFloat(), 0.0f, RandomFloat());
	rotation_axis_ = glm::normalize(rotation_axis_);
}

bool Particle::Update(float partial_time)
{
	rotation_ = angleAxis(180.0f * partial_time, rotation_axis_) * rotation_;
	return true;
}

ParticleSystem::~ParticleSystem()
{
	Clear();
}

void ParticleSystem::Init()
{
	quad_.Init(0.1f, 0.1f, glm::vec3(0, 1, 0));
}

void ParticleSystem::Add(Particle* particle)
{
	particles_.push_back(particle);
}

void ParticleSystem::Clear()
{
	for (const auto& ent : particles_)
		delete ent;
	particles_.clear();
}

void ParticleSystem::Render()
{
	auto shader = ShaderProgram::GetContextShader();
	for (const auto& particle : particles_)
	{
		ParticleMaterialTemplate.albedo = glm::vec4(particle->color_, 1.0f);
		BindMaterial(&ParticleMaterialTemplate);

		glm::mat4 model = glm::translate(glm::mat4(1.0f), particle->position_);
		model *= glm::toMat4(particle->rotation_);
		model = glm::scale(model, glm::vec3(particle->scale_));
		shader->SetUniform("model", model);
		quad_.Draw();
	}
}

void ParticleSystem::Update(float partial_time)
{
	auto iter = particles_.begin();
	while (iter != particles_.end())
	{
		Particle* particle = *iter;

		// update position, velocity
		particle->velocity_ += partial_time * glm::vec3(0.0f, -9.8f, 0.0f); // apply gravity
		particle->position_ += partial_time * particle->velocity_;

		// check if end of life-time
		if (!particle->Update(partial_time) || particle->age_ > particle->life_time_)
		{
			iter = particles_.erase(iter);
		}
		else
		{
			particle->age_ += partial_time;
			iter++;
		}
	}
}
