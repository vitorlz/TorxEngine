#pragma once

#include <unordered_map>
#include "model.h"
#include "Animator.h"

class AssetManager 
{
public:
	static Model& GetModel(const std::string name);
	static Mesh& GetMesh(const std::string name);
	static Animation& GetAnimation(const std::string name);
	static std::vector<Texture> LoadMeshTextures(const char* tag);
	static void LoadModels();
	static void LoadMeshes();
	static void LoadAnimations();
	static std::unordered_map<std::string, Model>& GetModelMap();
	static std::vector<std::string> GetModelNames();
	static Animation danceAnimation;
	static Animator animator;
	
private:
	static std::unordered_map<std::string, Model> m_Models;
	static std::unordered_map<std::string, Mesh> m_Meshes;
	static std::unordered_map<std::string, Animation> m_Animations;
	static std::vector<Texture> m_LoadedMeshTextures;

};

