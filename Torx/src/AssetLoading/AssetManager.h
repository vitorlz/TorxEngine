#pragma once

#include <unordered_map>
#include "model.h"

class AssetManager 
{
public:
	static Model& GetModel(std::string name);
	static Mesh& GetMesh(std::string name);
	static std::vector<Texture> LoadMeshTextures(const char* tag);
	static void LoadModels();
	static void LoadMeshes();
	static std::unordered_map<std::string, Model>& GetModelMap();
	static std::vector<std::string> GetModelNames();
private:
	static std::unordered_map<std::string, Model> m_Models;
	static std::unordered_map<std::string, Mesh> m_Meshes;
	static std::vector<Texture> m_LoadedMeshTextures;
};