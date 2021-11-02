#pragma once

class Animation
{
public:
	Animation(float from, float to, float duration_sec) 
		: from_(from), to_(to), duration_sec_(duration_sec) {}
	void Update(float partial_time);
	void SetRepeat(bool repeat);
	void SetReverse(bool reverse);
	void SetEaseMode(bool ease_in_mode);
	void Reset();
	bool isFinish();
	float GetValue();

private:
	bool is_repeat_ = false;
	bool is_reverse_ = false;
	bool is_ease_in_ = true;
	float frame_ = 1.0f;
	float from_;
	float to_;
	float duration_sec_;
};