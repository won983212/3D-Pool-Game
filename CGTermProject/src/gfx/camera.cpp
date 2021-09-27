#include <math.h>
#include "camera.h"
#include "../util/util.h"

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getFront() const
{
	return front;
}

glm::vec3 Camera::getRight() const
{
	return right;
}

void Camera::update()
{
	front = glm::vec3(0.0f, 0.0f, 0.0f);
	front.x = cos(DEGTORAD(yaw)) * cos(DEGTORAD(pitch));
	front.y = sin(DEGTORAD(pitch));
	front.z = sin(DEGTORAD(yaw)) * cos(DEGTORAD(pitch));

	front = glm::normalize(front);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}