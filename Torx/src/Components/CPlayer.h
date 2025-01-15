#pragma once 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Core/Common.h"

struct CPlayer 
{
	glm::mat4 viewMatrix;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
	bool flashlightOn;
	float movementSpeed;
};