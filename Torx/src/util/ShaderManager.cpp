#include "ShaderManager.h"
#include "memory"

std::unordered_map<std::string, Shader> ShaderManager::shaderPrograms{};


void ShaderManager::LoadShaders() 
{
	
	shaderPrograms.clear();

	Shader ourShader("res/shaders/testShader.vert", "res/shaders/testShader.frag");
	Shader cubemapShader("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");
	
	shaderPrograms.insert({ "ourShader", ourShader });
	shaderPrograms.insert({ "cubemapShader", cubemapShader });
}

Shader& ShaderManager::GetShaderProgram(std::string name) {
	return shaderPrograms[name];
}
