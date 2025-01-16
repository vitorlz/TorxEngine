#pragma once
#include <glm/glm.hpp>

enum ProjType
{
	ORTHO,
	PERSPECTIVE
};

struct CCamera
{
	ProjType projType;
	glm::mat4 projection;
	float fov;
	float near;
	float far;
	float left;
	float right;
	float bottom;
	float top;
};