#include "TextureLoader.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>
#include <vector>

unsigned int TextureLoader::LoadTexture(const char* path, const bool srgb) {

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data) {

		std::cout << path << ": " << nrComponents << std::endl;

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

unsigned int TextureLoader::LoadCubeMap(
	const std::string& faceRight, const std::string& faceLeft,
	const std::string& faceTop, const std::string& faceBottom,
	const std::string& faceBack, const  std::string& faceFront) 
{
	std::vector<std::string> faces{
		faceRight,
		faceLeft,
		faceTop,
		faceBottom,
		faceBack,
		faceFront
	};

	// load skybox textures. Note that we have to load one texture for each of the cube's faces. These textures will populate the 
	// texture object that is bound to GL_TEXTURE_CUBE_MAP with the cubemap textures.

	unsigned int textureID;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}