#pragma once

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

namespace Util
{
	std::vector<float> gaussian_weights(int kernel_size, float stdDeviation, float intervalMultiplier);
	void renderSphere();
	std::string vec3ToString(glm::vec3 vec);
};