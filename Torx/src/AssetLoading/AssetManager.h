#pragma once

#include <unordered_map>
#include "model.h"
#include "Animator.h"
#include "../Misc/TextRendering.h"
#include <IconsFontAwesome4.h>

struct ModelData
{
	std::string name;
	std::string path;
	Model model;
};

class AssetManager 
{
public:
	static Model& GetModel(const std::string name);
	static Mesh& GetMesh(const std::string name);
	static Animation& GetAnimation(const std::string name);
	static TextRendering& GetTextFont(const std::string name);
	static std::vector<Texture> LoadMeshTextures(const char* tag);
	static void LoadAssets();

	static std::vector<std::string> GetModelNames();	
	static std::vector<std::string> GetModelPaths();

	static std::string GetModelPath(std::string name);

	static void LoadModel(std::string path, std::string name);
	static void LoadAnimation(std::string path);
	static void LoadFont(std::string path);
private:
	static std::unordered_map<std::string, Mesh> m_Meshes;
	static std::unordered_map<std::string, Animation> m_Animations;
	static std::vector<Texture> m_LoadedMeshTextures;
	static std::unordered_map<std::string, TextRendering> m_textFonts;

	static std::vector<ModelData> m_Models;

	static void LoadMeshes();
	static void LoadAnimations();
	static void LoadFonts();
};

