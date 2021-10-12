#pragma once

#include "gfx.h"

namespace commoncg
{
	class Camera
	{
	public:
		Camera(glm::vec3 center, float yaw = -90.0f, float pitch = 0.0f, float zoom = 10.0f, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f))
			: center(center), worldUp(worldUp), yaw(yaw), pitch(pitch), zoom(zoom), front(glm::vec3(0.0f, 0.0f, -1.0f))
		{
			update();
		}
		Camera(float yaw = -90.0f, float pitch = 0.0f, float zoom = 10.0f)
			: Camera(glm::vec3(0.0f, 0.0f, 0.0f), yaw, pitch, zoom)
		{ }
		glm::mat4 getViewMatrix() const;
		glm::vec3 getFront() const;
		glm::vec3 getRight() const;
		glm::vec3 getUp() const;
		glm::vec3 getEyePosition() const;
		void update();
	public:
		glm::vec3 center;
		float zoom;
		float pitch;
		float yaw;
	private:
		glm::mat4 view;
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 worldUp;
		glm::vec3 eye;
	};
}