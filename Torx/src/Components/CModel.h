#pragma once

#include "../AssetLoading/model.h"

struct CModel
{
	Model model;
	std::string modelName;
	bool hasAOTexture{ true };
};