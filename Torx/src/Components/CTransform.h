#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


struct CTransform
{
	glm::vec3 position;
	glm::vec3 scale;
	glm::quat rotation;
};