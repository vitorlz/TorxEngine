#pragma once 
#include "../AssetLoading/mesh.h"

struct CMesh
{
	Mesh mesh;
	std::string meshType;
	std::string texture;
	bool isTextureDirty;
};