#pragma once

#include "Shader.h"
#include <unordered_map>
#include <iostream>

class ShaderManager {
public:
	
	static Shader& GetShaderProgram(std::string name);
	static void LoadShaders();
	static void ReloadShaders();

private:
	static std::unordered_map<std::string, std::shared_ptr<Shader>> shaderPrograms;
};