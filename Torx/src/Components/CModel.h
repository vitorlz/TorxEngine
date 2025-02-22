#pragma once

#include "../AssetLoading/model.h"

struct CModel
{
	Model model;
	std::string modelName;
	std::string path;
	bool hasAOTexture{ true };
};