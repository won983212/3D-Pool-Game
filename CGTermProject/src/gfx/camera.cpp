#include <math.h>
#include "camera.h"
#include "../util/util.h"

const float errorTolerance = 0.01f;
using namespace commoncg;

glm::mat4 Camera::getViewMatrix() const
{
	return view;
}

glm::vec3 Camera::getFront() const
{
	return front;
}

glm::vec3 Camera::getRight() const
{
	return right;
}

glm::vec3 Camera::getUp() const
{
	return up;
}

glm::vec3 Camera::getEyePosition() const
{
	return eye * zoom;
}

void Camera::update()
{
	// adjust pitch
	if (pitch > 90 - errorTolerance)
		pitch = 90 - errorTolerance;
	if (pitch < errorTolerance - 90)
		pitch = errorTolerance - 90;
	if (zoom < 2.0f)
		zoom = 2.0f;

	// calculate eye position
	eye.x = cos(DEGTORAD(yaw)) * cos(DEGTORAD(pitch));
	eye.y = sin(DEGTORAD(pitch));
	eye.z = sin(DEGTORAD(yaw)) * cos(DEGTORAD(pitch));
	eye = glm::normalize(eye);

	// get front, right, up
	front = -eye;
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	// get view matrix
	view = glm::lookAt(eye * zoom, center, up);
}