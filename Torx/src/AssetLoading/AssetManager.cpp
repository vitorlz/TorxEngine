#include "AssetManager.h"
#include "stb_image.h"
#include "../Util/TextureLoader.h"

std::unordered_map<std::string, Model> AssetManager::m_Models{};
std::unordered_map<std::string, Mesh> AssetManager::m_Meshes{};
std::vector<Texture> AssetManager::m_LoadedMeshTextures{};

void AssetManager::LoadModels()
{
	stbi_set_flip_vertically_on_load(true);
	//Model backpack("res/models/backpack/backpack.obj");
	stbi_set_flip_vertically_on_load(false);
	//Model debugCube("res/models/debugCube/scene.gltf");
	Model sponza("res/models/sponza-atrium/Sponza.gltf");
	Model victorianLamp("res/models/victorianLamp/scene.gltf");
	Model dirtBlock("res/models/dirtBlock/scene.gltf");
	//Model deagle("res/models/deagle/scene.gltf");
	//Model camera("res/models/camera/scene.gltf");
	//Model camera("res/models/camera/scene.gltf");
	//Model adamhead("res/models/adamhead/adamHead.gltf");
	//Model ar15("res/models/AR-15/scene.gltf");

	//mModels.insert({ "backpack", backpack });
    //mModels.insert({ "debugCube", debugCube });
	m_Models.insert({ "sponza", sponza });
	m_Models.insert({ "victorianLamp", victorianLamp });
	m_Models.insert({ "dirtBlock", dirtBlock });
	//mModels.insert({ "deagle", deagle });
	//mModels.insert({ "camera", camera });
	//mModels.insert({ "adamhead", adamhead });
	//mModels.insert({ "ar15", ar15 });

}

std::vector<Vertex> quadVertices = {
	// Position           // Normal          // TexCoords    // Tangent           // Bitangent
	{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-left
	{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-right
	{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Top-right
	{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}  // Top-left
};

std::vector<unsigned int> quadIndices = {
	0, 1, 2,  // First triangle
	2, 3, 0   // Second triangle
};

std::vector<Vertex> cubeVertices = {
	// Position           // Normal              // TexCoords    // Tangent           // Bitangent
	// Front face
	{{-0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-left
	{{ 0.5f, -0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-right
	{{ 0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Top-right
	{{-0.5f,  0.5f,  0.5f}, {0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Top-left

	// Back face
	{{ 0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-left
	{{-0.5f, -0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-right
	{{-0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Top-right
	{{ 0.5f,  0.5f, -0.5f}, {0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Top-left

	// Left face
	{{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-left
	{{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-right
	{{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}, // Top-right
	{{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f, 0.0f}}, // Top-left

	// Right face
	{{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-left
	{{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom-right
	{{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, // Top-right
	{{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}}, // Top-left

	// Top face
	{{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}, {1.0f,  0.0f, 0.0f}, {0.0f,  0.0f, -1.0f}}, // Bottom-left
	{{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}, {1.0f,  0.0f, 0.0f}, {0.0f,  0.0f, -1.0f}}, // Bottom-right
	{{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}, {1.0f,  0.0f, 0.0f}, {0.0f,  0.0f, -1.0f}}, // Top-right
	{{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}, {1.0f,  0.0f, 0.0f}, {0.0f,  0.0f, -1.0f}}, // Top-left

	// Bottom face
	{{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}, {1.0f,  0.0f, 0.0f}, {0.0f,  0.0f,  1.0f}}, // Bottom-left
	{{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}, {1.0f,  0.0f, 0.0f}, {0.0f,  0.0f,  1.0f}}, // Bottom-right
	{{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}, {1.0f,  0.0f, 0.0f}, {0.0f,  0.0f,  1.0f}}, // Top-right
	{{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}, {1.0f,  0.0f, 0.0f}, {0.0f,  0.0f,  1.0f}}, // Top-left
};

std::vector<unsigned int> cubeIndices = {
	// Front face
	0, 1, 2, 2, 3, 0,
	// Back face
	4, 5, 6, 6, 7, 4,
	// Left face
	8, 9, 10, 10, 11, 8,
	// Right face
	12, 13, 14, 14, 15, 12,
	// Top face
	16, 17, 18, 18, 19, 16,
	// Bottom face
	20, 21, 22, 22, 23, 20
};

std::vector<Texture> AssetManager::LoadMeshTextures(const char* tag)
{
	std::vector<Texture> textures;
	bool texturesAlreadyLoaded{ false };

	for (Texture texture : m_LoadedMeshTextures)
	{
		if (texture.path == tag)
		{
			texturesAlreadyLoaded = true;
			textures.push_back(texture);
		}
	}

	
	if (texturesAlreadyLoaded)
	{
		return textures;
	}

	std::string tagString(tag);

	std::string albedoPath = "res/textures/pbr/" + tagString + "/" + tagString + "_albedo.png";
	std::string normalPath = "res/textures/pbr/" + tagString + "/" + tagString + "_normal.png";

	textures.push_back({ TextureLoader::LoadRMATexture(tag), "texture_rma", tag });
	textures.push_back({ TextureLoader::LoadTexture(albedoPath.c_str(), true), "texture_albedo", tag });
	textures.push_back({ TextureLoader::LoadTexture(normalPath.c_str(), false), "texture_normal", tag });

	m_LoadedMeshTextures.insert(m_LoadedMeshTextures.end(), textures.begin(), textures.end());

	return textures;
	
}

void AssetManager::LoadMeshes()
{	
	Mesh cube(cubeVertices, cubeIndices, LoadMeshTextures("darkmarble"));
	Mesh quad(quadVertices, quadIndices, LoadMeshTextures("darkmarble"));

	m_Meshes.insert({ "cube", cube });
	m_Meshes.insert({ "quad", quad });
}

Model& AssetManager::GetModel(std::string name)
{
	return m_Models[name];
}

Mesh& AssetManager::GetMesh(std::string name)
{
	return m_Meshes[name];
}

std::unordered_map<std::string, Model>& AssetManager::GetModelMap()
{
	return m_Models;
}

std::vector<std::string> AssetManager::GetModelNames()
{
	std::vector<std::string> names{};
	for (auto& pair : m_Models)
	{
		names.push_back(pair.first);

		
	}
	return names;
}