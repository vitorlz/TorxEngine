#pragma once 

struct CPlayer 
{
	glm::mat4 viewMatrix;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;
	bool flashlightOn;
};