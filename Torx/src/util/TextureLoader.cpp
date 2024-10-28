#include "TextureLoader.h"
#include <glad/glad.h>
#include <stb_image.h>
#include <iostream>

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