#pragma once

#include <glm/glm.hpp>


struct CTransform
{
	glm::vec3 position;
	glm::vec3 scale;
	glm::vec3 rotation;
	glm::mat4 rotationMatrix;
};