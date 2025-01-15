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
	//float fov;
	//float far;
	//float near;
	//float width;
	//float height;
};