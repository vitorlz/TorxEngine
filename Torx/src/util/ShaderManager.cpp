#include "ShaderManager.h"
#include "memory"
#include <vector>

std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderManager::shaderPrograms;


void ShaderManager::LoadShaders() 
{

	std::shared_ptr<Shader> ourShader =  std::make_shared<Shader>("res/shaders/testShader.vert", "res/shaders/testShader.frag");
	std::shared_ptr<Shader> cubemapShader = std::make_shared<Shader>("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");

	shaderPrograms.clear();

	shaderPrograms.insert({ "ourShader", ourShader });
	shaderPrograms.insert({ "cubemapShader", cubemapShader });
}

Shader& ShaderManager::GetShaderProgram(std::string name) 
{
	return *shaderPrograms[name];
}

void ShaderManager::ReloadShaders() 
{
	std::cout << "Reloading Shaders..\n";

	std::vector<std::shared_ptr<Shader>> pointersToDelete;

	for (auto& pair : shaderPrograms)
	{
	
		glDeleteProgram(pair.second->ID);
		pointersToDelete.push_back(pair.second);
	}

	ShaderManager::LoadShaders();

	for (auto& shaderPtr : pointersToDelete)
	{
		shaderPtr.reset();
	}

	std::cout << "Shaders Reloaded \n";
}
