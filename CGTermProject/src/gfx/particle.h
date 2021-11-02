#pragma once

#include <vector>
#include "gfx.h"
#include "../model/quad.h"

namespace commoncg
{
	class Particle 
	{
	public:
		Particle() : position_(0.0f), velocity_(0.0f) {}
		bool Update(float partial_time);
		void SetLifeTime(float life_time) { life_time_ = life_time; }

	public:
		glm::vec3 position_;
		glm::vec3 velocity_;
		glm::quat rotation_;

	private:
		float age_ = 0.0f;
		float life_time_ = 1.0f;
		friend class ParticleSystem;
	};

	class ParticleSystem
	{
	public:
		~ParticleSystem();
		void Init();
		void Add(Particle* particle);
		void Clear();
		void Render();
		void Update(float partial_time);

	private:
		std::vector<Particle*> particles_;
		model::Quad quad_;
	};
}