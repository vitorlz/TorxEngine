#include "AssetManager.h"
#include "stb_image.h"

std::unordered_map<std::string, Model> AssetManager::mModels{};


void AssetManager::LoadModels()
{
	stbi_set_flip_vertically_on_load(true);
	Model backpack("res/models/backpack/backpack.obj");

	mModels.insert({ "backpack", backpack });
}

Model& AssetManager::GetModel(std::string name)
{
	return mModels[name];
}