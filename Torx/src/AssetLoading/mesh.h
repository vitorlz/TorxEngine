#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "../Util/Shader.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;

	// bone indices influencing this vertex
	int m_BoneIDs[MAX_BONE_INFLUENCE];
	// weights from each bone (how much each bone affects this vertex)
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
	unsigned int id; // texture's id.
	std::string type; // texture type (diffuse, specular, etc...)
	std::string path; // we store the path of the texture to compare with other textures
};

class Mesh {
public:

	Mesh();

	// mesh data
	std::vector<Vertex> vertices; 
	std::vector<unsigned int> indices; // the indices of the vertices (can render via an index buffer for efficiency)
	std::vector<Texture> textures;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);
	void Draw(Shader& shader);

private:
	// render data
	unsigned int VAO, VBO, EBO;

	void setupMesh();

};
