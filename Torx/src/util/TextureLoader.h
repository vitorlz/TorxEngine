#pragma once

#include <iostream>

class TextureLoader {
public:
	static unsigned int LoadTexture(const char* path, const bool srgb);
	static unsigned int LoadCubeMap(
		const std::string& faceRight, const std::string& faceLeft,
		const std::string& faceTop, const std::string& faceBottom,
		const std::string& faceBack, const  std::string& faceFront);
};