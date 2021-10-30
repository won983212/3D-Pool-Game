#include <cmath>
#include "camera.h"
#include "../util/util.h"

constexpr float ErrorTolerance = 0.01f;

using namespace commoncg;

bool Camera::GetViewMatrix(glm::mat4* view)
{
	const bool ret = dirty_;
	*view = this->view_;
	dirty_ = false;
	return ret;
}

glm::vec3 Camera::GetFront() const
{
	return front_;
}

glm::vec3 Camera::GetRight() const
{
	return right_;
}

glm::vec3 Camera::GetUp() const
{
	return up_;
}

glm::vec3 Camera::GetEyePosition() const
{
	return eye_;
}

void Camera::Update()
{
	// adjust pitch
	if (pitch_ > 90 - ErrorTolerance)
		pitch_ = 90 - ErrorTolerance;
	if (pitch_ < ErrorTolerance - 90)
		pitch_ = ErrorTolerance - 90;
	if (zoom_ < 2.0f)
		zoom_ = 2.0f;

	// calculate eye position
	eye_.x = cos(DEGTORAD(yaw_)) * cos(DEGTORAD(pitch_));
	eye_.y = sin(DEGTORAD(pitch_));
	eye_.z = sin(DEGTORAD(yaw_)) * cos(DEGTORAD(pitch_));
	eye_ *= zoom_;

	// get front, right, up
	front_ = -normalize(eye_);
	right_ = normalize(cross(front_, world_up_));
	up_ = normalize(cross(right_, front_));

	// get view matrix
	eye_ += center_;
	view_ = lookAt(eye_, center_, up_);
	dirty_ = true;
}
