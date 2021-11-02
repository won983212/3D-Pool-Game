#include "particle.h"
#include "shader.h"
#include "../model/model.h"
#include "../util/util.h"

using namespace commoncg;

constexpr model::Material ParticleMaterial =
{
	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), // albedo
	0.1f, // metallic
	0.1f, // roughness
	DefaultAo, // ao
	{0, -1, -1, -1} // texIndex
};

bool Particle::Update(float partial_time)
{
	// TODO Apply rotation
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
	BindMaterial(&ParticleMaterial);
	auto shader = ShaderProgram::GetContextShader();
	for (const auto& particle : particles_)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), particle->position_);
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
		particle->velocity_ += partial_time * glm::vec3(0.0f, -0.98f, 0.0f); // apply gravity
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
