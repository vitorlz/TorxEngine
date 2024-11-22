#pragma once 
#include "../AssetLoading/mesh.h"

struct CMesh
{
	Mesh mesh;
	std::string meshType;
	std::string texture;
	glm::vec2 textureScaling{ 1.0f };
};