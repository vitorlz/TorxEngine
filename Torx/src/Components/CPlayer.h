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
	glm::mat4 projectionMatrix
	{ 
		glm::perspective(glm::radians(45.0f), (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT, 0.1f, 100.0f) 
	};
	bool flashlightOn;
	float movementSpeed;
};