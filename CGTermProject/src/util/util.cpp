#include <random>
#include "util.h"

std::random_device rd;
std::mt19937 r_engine(rd());
std::uniform_real_distribution<> real_dist(0, 1);


float RandomFloat()
{
	return real_dist(r_engine);
}

float RandomFloat(float min, float max)
{
	return real_dist(r_engine) * (max - min) + min;
}