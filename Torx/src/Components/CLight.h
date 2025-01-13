#pragma once

#include <glm/glm.hpp>
#include <iostream>

enum LightType
{
	DIRECTIONAL = 0,
	POINT = 1,
	SPOT = 2,
};

struct CLight
{
	LightType type;
	glm::vec3 color;
	float radius;
	float strength;

	// for spotlight
	glm::vec3 direction;
	float innerCutoff;
	float outerCutoff;
	bool shadowCaster{ false };
	// set isDirty to true initially so light is updated at least once
	bool isDirty{ true };
	glm::vec3 offset;

	bool dynamic{ false };

};


