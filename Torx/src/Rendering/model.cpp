#include "model.h"
#include "stb_image.h"
#include <filesystem>

std::vector<Texture> textures_loaded;

Model::Model(const char* path) {
	Model::loadModel(path);
}

// iterate over each of the meshes to call their respective draw function
void Model::Draw(Shader& shader){

	for (unsigned int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
	}

	// CAUTION! this is only called if the model is drawn. This means that if we load the model by simply
	// creating a model object like Model model("somepath") the textures are going to be loaded, and the uniforms are going
	// to be set and the uniforms are never going to be reset.

	shader.setInt("material.texture_diffuse1", 31);
	shader.setInt("material.texture_specular1", 31);
	shader.setInt("material.texture_emission1", 31);
	shader.setInt("material.texture_normal1", 31);
}

void Model::loadModel(std::string path) {

	// read a file into a scene object. Inside the scene object assimp will organize the data in its own datastructures.
	// we use the scene object to access all of the other datastructures.
	Assimp::Importer import;
	const aiScene *scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	directory = path.substr(0, path.find_last_of('/'));
	
	processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene, glm::mat4 parentTransform) {

	glm::mat4 nodeTransform = Model::convertToGLMMatrix(node->mTransformation);

	// this is the global transformation. For the root node, parentTransform is going to me the identity matrix. 
	// Which means that globalTransform = nodeTransform for the Root node. 
	glm::mat4 globalTransform = parentTransform * nodeTransform;
	
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		// get the mesh from the scene object's mesh array based on the index in the nodes' mesh index array;
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		// process mesh and the returned mesh object to the mesh array 
		meshes.push_back(processMesh(mesh, scene, globalTransform));
	}


	for (unsigned int i = 0; i < node->mNumChildren; i++) {

		// we recursively pass the globalTransform (which in the first iteration is the transform of the root node) to each child node.
		// then the same process happens again: the globaltransform gets multiplied by the node transform and passed down to any other children.
		// this means that we are essentially transforming 
		processNode(node->mChildren[i], scene, globalTransform);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 globalTransform) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
		
		Vertex vertex;
		// process vertex positions, normals and texture coordinates that are stored in the mesh 
		
		// in assimp the vertex position array is called mVertices.
		glm::vec3 vector;
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;
		vertex.Position = glm::vec3(globalTransform * glm::vec4(vector, 1.0f));

		// process the normals

		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;
			vertex.Normal = glm::mat3(glm::transpose(glm::inverse(globalTransform))) * vector;
		}

		// Texture coordinates are roughly the same, but Assimp allows a model to have up to 8 different texture coordinates per vertex.
		// We're not going to use 8, we only care about the first set of texture coordinates. 
		// We'll also want to check if the mesh actually contains texture coordinates(which may not be always the case):

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;

			// maybe have to multiply by global transform will see.
			vector.x = mesh->mTangents[i].x;
			vector.y = mesh->mTangents[i].y;
			vector.z = mesh->mTangents[i].z;
			vertex.Tangent = glm::mat3(glm::transpose(glm::inverse(globalTransform))) * vector;

			vector.x = mesh->mBitangents[i].x;
			vector.x = mesh->mBitangents[i].y;
			vector.x = mesh->mBitangents[i].z;
			vertex.Bitangent = glm::mat3(glm::transpose(glm::inverse(globalTransform)))* vector;
		} 
		else {
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}

		vertices.push_back(vertex);
	}

	// process indices (which are stored separately from the vertex data just like we had in our program)
	// remember that each mesh has an array of faces, where each face represents a single primitive, these contain the indices of the vertices
	// in our case, these will always be triangles because of the aiProcess_Triangulate option we set.

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	// a mesh contains an index to a material object (that has the actual colors (diffuse map and specular map)) of the object.
	// To retrieve the material of a mesh we need to index the scene's mMaterials array. The mesh material index is set in its mMaterialIndex property.
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		// materials store the texture locations in arrays for each texture type.
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		// insert function is used to insert elements into a vector. textures.end() specified where to put the elements (at the end of the vector)
		// diffuseMaps.begin() marks the start of the range of elements to be inserted into textures and diffuseMaps.end() marks the end. This is useful for inserting vectors into other vectors.
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// metallness because almost no model comes with specular textures
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<Texture> emissionMaps = loadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emission");
		textures.insert(textures.end(), emissionMaps.begin(), emissionMaps.end());

		std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	}

	return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++) {
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		// check if texture has already been loaded
		for (unsigned int j = 0; j < textures_loaded.size(); j++) {
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{ // if texture hasn't been loaded already, load it.
			Texture texture;
			// THIS FUNCTION ONLY WORKS IF THE TEXTURES ARE STORED IN THE SAME DIRECTORY AS THE MODEL ITSELF
			// SOME MODELS STORE TEXTURES IN ABSOLUTE PATHS, WHICH WE NEED TO EDIT TO LOCAL IF WE WANT IT TO WORK

			if (typeName == "texture_diffuse")
			{
				texture.id = TextureFromFile(str.C_Str(), directory, true);
			}
			else
			{
				texture.id = TextureFromFile(str.C_Str(), directory, false);
			}

			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // add to loaded textures
		}
	}

	return textures;
}

// THIS FUNCTION ONLY WORKS IF THE TEXTURES ARE STORED IN THE SAME DIRECTORY AS THE MODEL ITSELF
// SOME MODELS STORE TEXTURES IN ABSOLUTE PATHS, WHICH WE NEED TO EDIT TO LOCAL IF WE WANT IT TO WORK
unsigned int TextureFromFile(const char* path, const std::string& directory, const bool srgb) {
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {

		std::cout << filename.c_str() << ": " << nrComponents << std::endl;
		GLenum storeFormat;
		GLenum dataFormat;

		if (nrComponents == 1) {
			dataFormat = GL_RED;
			storeFormat = GL_RED;
		}
		else if (nrComponents == 3) {
			dataFormat = GL_RGB;
			storeFormat = srgb ? GL_SRGB : GL_RGB; // here we are assuming that the texture is already gamma corrected and we are transforming it back to
												   // a linear space because we want to apply gamma correction ourselves in the shaders as the last step
												   // so that we can do our intermediate color/light calculations in a linear space.
		}
		else if (nrComponents == 4) {
			dataFormat = GL_RGBA;
			storeFormat = srgb ? GL_SRGB_ALPHA : GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, storeFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


unsigned int TextureFromFile(const char* path, const bool srgb) {
	std::string filename = std::string(path);

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data) {

		std::cout << filename.c_str() << ": " << nrComponents << std::endl;

		GLenum storeFormat;
		GLenum dataFormat;

		if (nrComponents == 1) {
			dataFormat = GL_RED;
			storeFormat = GL_RED;
		}
		else if (nrComponents == 3) {
			dataFormat = GL_RGB;
			storeFormat = srgb ? GL_SRGB : GL_RGB;
		}
		else if (nrComponents == 4) {
			dataFormat = GL_RGBA;
			storeFormat = srgb ? GL_SRGB_ALPHA : GL_RGBA;
		}

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, storeFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



		stbi_image_free(data);
	}
	else {
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

glm::mat4 Model::convertToGLMMatrix(const aiMatrix4x4& matrix) {
	glm::mat4 glmMatrix;

	glmMatrix[0][0] = matrix.a1; glmMatrix[1][0] = matrix.a2; glmMatrix[2][0] = matrix.a3; glmMatrix[3][0] = matrix.a4;
	glmMatrix[0][1] = matrix.b1; glmMatrix[1][1] = matrix.b2; glmMatrix[2][1] = matrix.b3; glmMatrix[3][1] = matrix.b4;
	glmMatrix[0][2] = matrix.c1; glmMatrix[1][2] = matrix.c2; glmMatrix[2][2] = matrix.c3; glmMatrix[3][2] = matrix.c4;
	glmMatrix[0][3] = matrix.d1; glmMatrix[1][3] = matrix.d2; glmMatrix[2][3] = matrix.d3; glmMatrix[3][3] = matrix.d4;

	return glmMatrix;
}