#include <iostream>
#include <vector>
#include "../Util/Shader.h"
#include "mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Model {
public:
	Model(const char* path);
	void Draw(Shader& shader);
private:
	// model data
	std::vector<Mesh> meshes;
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform = glm::mat4(1.0f));
	glm::mat4 convertToGLMMatrix(const aiMatrix4x4& matrix);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 globalTransform);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

unsigned int TextureFromFile(const char* path, const std::string& directory, const bool gammaCorrection);
unsigned int TextureFromFile(const char* path, const bool gammaCorrection);