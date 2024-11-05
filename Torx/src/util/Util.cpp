#include "Util.h"
#include "../Core/Common.h"
#include <numbers>

std::vector<float> Util::gaussian_weights(int kernel_size, float stdDeviation, float intervalMultiplier)
{
	float variance = stdDeviation * stdDeviation;
	float weight;
	float x = 1;
	std::vector<float> weights;

	for (float i = 1; i <= kernel_size; i++)
	{

		weight = (1 / pow(2 * std::numbers::pi * variance, 0.5)) * exp(-((x * x) / (2 * variance)));
		weights.push_back(weight);

		x += intervalMultiplier * i;
	}

	return weights;
};