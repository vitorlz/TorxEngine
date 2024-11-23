#pragma once
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "../Util/Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
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
