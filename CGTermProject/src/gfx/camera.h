#pragma once

#include "gfx.h"

namespace commoncg
{
	class Camera
	{
	public:
		Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f))
			: position(position), worldUp(worldUp), yaw(yaw), pitch(pitch), front(glm::vec3(0.0f, 0.0f, -1.0f))
		{
			update();
		}
		Camera(float posX, float posY, float posZ, float yaw = -90.0f, float pitch = 0.0f)
			: Camera(glm::vec3(posX, posY, posZ), yaw, pitch)
		{ }
		glm::mat4 getViewMatrix() const;
		glm::vec3 getFront() const;
		glm::vec3 getRight() const;
		void update();
	public:
		glm::vec3 position;
		float pitch;
		float yaw;
	private:
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 right;
		glm::vec3 worldUp;
	};
}