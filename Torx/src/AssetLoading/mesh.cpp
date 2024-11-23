#include "mesh.h"
#include <cstddef>
#include <string>
#include <glad/glad.h>

Mesh::Mesh() {};
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices) {};

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures) {
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	setupMesh();
}

void Mesh::setupMesh() {
	// initialize buffers / VAO, VBO, and EBO
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
	// vertex tangent 
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
	// vertex bitangent 
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

	// unbind VAO. This VAO has state configurations for a single mesh.
	glBindVertexArray(0);
}

// we have to pass the shader object by reference because that will prevent copying and we want to modify the original inside the function.
void Mesh::Draw(Shader &shader) 
{

	shader.setInt("material.texture_albedo1", 31);
	shader.setInt("material.texture_emission1", 31);
	shader.setInt("material.texture_rma1", 31);
	shader.setInt("material.texture_normal1", 31);

	unsigned int emissionNr = 1;
	unsigned int normalNr = 1;
	unsigned int albedoNr = 1;
	unsigned int rmaNr = 1;
	unsigned int ambientNr = 1;
	
	unsigned int i = 0;

	std::string number;
	std::string name; 

	for (i = 0; i < textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding 

		// retrieve texture number (the N in texture_diffuseN)
		number;
		name = textures[i].type;
		
		if (name == "texture_emission")
			number = std::to_string(emissionNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_albedo")
			number = std::to_string(albedoNr++);
		else if (name == "texture_rma")
			number = std::to_string(rmaNr++);	

		shader.setInt(("material." + name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}