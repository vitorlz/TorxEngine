#include "AssetManager.h"
#include "stb_image.h"
#include "../Util/TextureLoader.h"

std::unordered_map<std::string, Model> AssetManager::m_Models{};
std::unordered_map<std::string, Mesh> AssetManager::m_Meshes{};
std::unordered_map<std::string, Animation> AssetManager::m_Animations;
std::vector<Texture> AssetManager::m_LoadedMeshTextures{};
std::unordered_map<std::string, TextRendering>  AssetManager::m_textFonts;

void AssetManager::LoadModels()
{
	
	stbi_set_flip_vertically_on_load(false);
	
	Model sponza("res/models/sponza-atrium/Sponza.gltf");
	Model ceilingLamp1("res/models/ceilingLamp1/scene.gltf");
	Model camera("res/models/camera/scene.gltf");
	Model zombie("res/models/zombie/zombie.gltf");
	
	m_Models.insert({ "zombie", zombie });
	m_Models.insert({ "sponza", sponza });
	m_Models.insert({ "ceilingLamp1", ceilingLamp1 });
	m_Models.insert({ "camera", camera });
}

void AssetManager::LoadAnimations()
{
	// Each animated model has to have one animator and can possibly have many animations. When we want to play another animation we can simply call Animator::PlayAnimation.
	// All animators have to be updated every frame.
	// Notice that when we create an animation we have to pass in a model. That model has to be the skinned model.
	// That model will have the bone weights, vertex Id's, bone offset matrices, bone name, etc... 
	// So for example if we are loading animations for the player, the skinned model will always be the same and the animations can change.
	// The animator is used to update the animations (calculate the position of the bones for the current frame) and get the final bone matrices, which we send to the vertex shader.
	// we can have many systems triggering animations. Like a player behavior system that handles player state and also triggers animations such as shooting, walking, etc... The animator
	// should be a component. The animation system should just update all animator components. These behavior systems that trigger animations will also add the animator component to the entity and 
	// initialize it with some default animation in their init() function. In the update() function these systems will change the animation based on the game state. For example, an EnemyBehavior
	// system will add an animator component to the enemy initialized with some idle animation in its init() function. Then in its update() function it will change the animation based
	// on enemy state (attack, run, etc...)

	m_Animations.insert({ "zombieDance", Animation("res/models/zombie/zombie.gltf", &GetModel("zombie"))});

}

void AssetManager::LoadFonts()
{
	TextRendering texGyreCursor;
	TextRendering arial;

	texGyreCursor.LoadFont("res/fonts/texgyrecursor/texgyrecursor-bold.otf");
	arial.LoadFont("res/fonts/arial/arial.ttf");

	m_textFonts.insert({ "texGyreCursor", texGyreCursor });
	m_textFonts.insert({ "arial", arial });
}

TextRendering& AssetManager::GetTextFont(const std::string name)
{
	return m_textFonts[name];
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

Model& AssetManager::GetModel(const std::string name)
{
	return m_Models[name];
}

Mesh& AssetManager::GetMesh(const std::string name)
{
	return m_Meshes[name];
}

Animation& AssetManager::GetAnimation(const std::string name)
{
	return m_Animations[name];
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