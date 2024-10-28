#pragma once

#include "Shader.h"
#include <unordered_map>
#include <iostream>

class ShaderManager {
public:
	
	static Shader& GetShaderProgram(std::string name);
	static void LoadShaders();

private:
	static std::unordered_map<std::string, Shader> shaderPrograms;
};