#pragma once

#include "gfx.h"

namespace commoncg
{
	class Camera
	{
	public:
		Camera(glm::vec3 center, float yaw = -90.0f, float pitch = 0.0f, float zoom = 10.0f, glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f))
			: center_(center), zoom_(zoom), pitch_(pitch), yaw_(yaw), front_(glm::vec3(0.0f, 0.0f, -1.0f)), world_up_(world_up)
		{
			Update();
		}

		Camera(float yaw = -90.0f, float pitch = 0.0f, float zoom = 10.0f)
			: Camera(glm::vec3(0.0f, 0.0f, 0.0f), yaw, pitch, zoom)
		{
		}

		// return: is it changed?
		bool GetViewMatrix(glm::mat4* view);
		glm::vec3 GetFront() const;
		glm::vec3 GetRight() const;
		glm::vec3 GetUp() const;
		glm::vec3 GetEyePosition() const;
		void Update();

	public:
		glm::vec3 center_;
		float zoom_;
		float pitch_;
		float yaw_;

	private:
		glm::mat4 view_;
		glm::vec3 front_;
		glm::vec3 up_;
		glm::vec3 right_;
		glm::vec3 world_up_;
		glm::vec3 eye_;
		bool dirty_ = true;
	};
}
