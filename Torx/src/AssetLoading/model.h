#pragma once

#include <iostream>
#include <vector>
#include "../Util/Shader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>
#include <vector>

struct BoneInfo
{
	// id id used as index in finalBoneMatrices
	int id;

	// offset matrix transforms vertex from model space to bone space
	glm::mat4 offset;
};

class Model {
public:
	Model();
	Model(const char* path);
	void Draw(Shader& shader);

	std::vector<Mesh> meshes;

	std::map<std::string, BoneInfo>& GetBoneInfoMap();
	int& GetBoneCount();
private:

	// skeletal animation stuff
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int m_BoneCounter = 0;
	
	void SetVertexBoneDataToDefault(Vertex& vertex);
	void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
	void ExtractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
	
	// model data
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform = glm::mat4(1.0f));
	glm::mat4 convertToGLMMatrix(const aiMatrix4x4& matrix);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 globalTransform);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

unsigned int TextureFromFile(const char* path, const std::string& directory, const bool srgb);
