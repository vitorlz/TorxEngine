#pragma once

#include <vector>

class Util
{
public:
	static std::vector<float> gaussian_weights(int kernel_size, float stdDeviation, float intervalMultiplier);
	static void renderSphere();
};