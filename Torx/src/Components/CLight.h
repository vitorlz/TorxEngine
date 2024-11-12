#pragma once

#include <glm/glm.hpp>
#include <iostream>

enum LightType
{
	DIRECTIONAL = 0,
	POINT = 1,
	SPOT = 2,
	FLASHLIGHT= 3
};

struct CLight
{
	LightType type;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float radius;

	// for spotlight
	glm::vec3 direction;
	float innerCutoff;
	float outerCutoff;
	int shadowCaster{ 0 };
	// set isDirty to true initially so light is updated at least once
	int isDirty{ 1 };
	glm::vec3 offset;
};


