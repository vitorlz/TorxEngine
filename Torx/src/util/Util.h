#pragma once

#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include "../Core/Types.hpp"

namespace Util
{
	std::vector<float> gaussian_weights(int kernel_size, float stdDeviation, float intervalMultiplier);
	void renderSphere();
	std::string vec3ToString(glm::vec3 vec);
	Entity duplicateEntity(Entity entity);
	void decomposeMtx(const glm::mat4& m, glm::vec3& pos, glm::quat& rot, glm::vec3& scale);
};