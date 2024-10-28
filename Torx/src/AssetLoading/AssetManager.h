#pragma once

#include <unordered_map>
#include "model.h"

class AssetManager 
{
public:
	static Model& GetModel(std::string name);
	static void LoadModels();

private:
	static std::unordered_map<std::string, Model> mModels;
};