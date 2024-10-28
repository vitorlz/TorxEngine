#pragma once

#include <unordered_map>
#include "model.h"

class AssetManager 
{
public:
	static Model& GetModel(std::string name);
	static void LoadModels();
	static unsigned int CreateCubeVao();
	static unsigned int CreateQuadVao();

private:
	static std::unordered_map<std::string, Model> mModels;
};