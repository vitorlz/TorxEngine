#pragma once

#include <glm/glm.hpp>
#include <iostream>

enum LightType
{
	POINT,
	DIRECTIONAL,
	SPOT,
	FLASHLIGHT
};

struct CLight
{
	LightType type;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float quadratic;

	// for spotlight
	glm::vec3 direction;
	float innerCutoff;
	float outerCutoff;
};


