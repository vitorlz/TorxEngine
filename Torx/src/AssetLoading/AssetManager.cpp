#include "AssetManager.h"
#include "stb_image.h"

std::unordered_map<std::string, Model> AssetManager::mModels{};

void AssetManager::LoadModels()
{
	stbi_set_flip_vertically_on_load(true);
	Model backpack("res/models/backpack/backpack.obj");
	stbi_set_flip_vertically_on_load(false);
    Model debugCube("res/models/debugCube/scene.gltf");
    Model sponza("res/models/sponza-atrium/Sponza.gltf");

	mModels.insert({ "backpack", backpack });
    mModels.insert({ "debugCube", debugCube });
    mModels.insert({ "sponza", sponza });
}

Model& AssetManager::GetModel(std::string name)
{
	return mModels[name];
}
