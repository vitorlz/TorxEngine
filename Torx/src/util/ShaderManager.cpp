#include "ShaderManager.h"
#include "memory"

std::unordered_map<std::string, Shader> ShaderManager::shaderPrograms{};


void ShaderManager::LoadShaders() 
{
	
	shaderPrograms.clear();

	Shader ourShader("res/shaders/testShader.vert", "res/shaders/testShader.frag");
	
	shaderPrograms.insert({ "ourShader", ourShader });
}

Shader& ShaderManager::GetShaderProgram(std::string name) {
	return shaderPrograms[name];
}
