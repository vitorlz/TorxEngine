#pragma once

#include <glm/glm.hpp>

struct CLight
{
	bool directionalLight;
	bool pointLight;
	bool spotLight;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float quadratic;
};


