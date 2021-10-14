#include <math.h>
#include "camera.h"
#include "../util/util.h"

const float errorTolerance = 0.01f;
using namespace commoncg;

bool Camera::getViewMatrix(glm::mat4* view)
{
	bool ret = dirty;
	*view = this->view;
	dirty = false;
	return ret;
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
	return eye;
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
	eye *= zoom;

	// get front, right, up
	front = -glm::normalize(eye);
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));

	// get view matrix
	eye += center;
	view = glm::lookAt(eye, center, up);
	dirty = true;
}