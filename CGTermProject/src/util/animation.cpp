#include "animation.h"
#include <math.h>

void Animation::Update(float partial_time)
{
	frame_ += partial_time / duration_sec_;
	if (frame_ > 1.0f)
	{
		frame_ = is_repeat_ ? 0.0f : 1.0f;
	}
}

void Animation::SetReverse(bool reverse)
{
	is_reverse_ = reverse;
}

void Animation::SetEaseMode(bool ease_in_mode)
{
	is_ease_in_ = ease_in_mode;
}

void Animation::SetRepeat(bool repeat)
{
	is_repeat_ = repeat;
}

void Animation::Reset()
{
	frame_ = 0;
}

bool Animation::isFinish()
{
	return frame_ == 1.0f;
}

float Animation::GetValue()
{
	// calculate frame (0.0 - 1.0) and map to (from_ - to_)
	float frame = is_reverse_ ? (1.0f - frame_) : frame_;
	frame = frame * (to_ - from_) + from_;

	if (is_ease_in_)
	{
		frame = pow(frame, 4.0f); // map to x^4
	}
	else
	{
		frame = 1 - pow(frame - 1.0f, 4.0f); // map to 1 - (x - 1)^4
	}

	return frame;
}