#include "ShaderManager.h"
#include "memory"
#include <vector>

std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderManager::shaderPrograms;


void ShaderManager::LoadShaders() 
{

	std::shared_ptr<Shader> ourShader =  std::make_shared<Shader>("res/shaders/testShader.vert", "res/shaders/testShader.frag");
	std::shared_ptr<Shader> cubemapShader = std::make_shared<Shader>("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");
	std::shared_ptr<Shader> lightingShader = std::make_shared<Shader>("res/shaders/lighting.vert", "res/shaders/lighting.frag");
	std::shared_ptr<Shader> solidColorShader = std::make_shared<Shader>("res/shaders/solidColor.vert", "res/shaders/solidColor.frag");
	std::shared_ptr<Shader> postProcessingShader = std::make_shared<Shader>("res/shaders/postProcessing.vert", "res/shaders/postProcessing.frag");
	std::shared_ptr<Shader> pointShadowMapShader = std::make_shared<Shader>("res/shaders/pointShadowMap.vert", "res/shaders/pointShadowMap.frag", "res/shaders/pointShadowMap.geom");
	std::shared_ptr<Shader> blurShader = std::make_shared<Shader>("res/shaders/gaussianBlur.vert", "res/shaders/gaussianBlur.frag");
	std::shared_ptr<Shader> pbrLightingShader = std::make_shared<Shader>("res/shaders/lighting.vert", "res/shaders/pbrLighting.frag");

	shaderPrograms.clear();

	shaderPrograms.insert({ "ourShader", ourShader });
	shaderPrograms.insert({ "cubemapShader", cubemapShader });
	shaderPrograms.insert({ "lightingShader", lightingShader });
	shaderPrograms.insert({ "solidColorShader", solidColorShader });
	shaderPrograms.insert({ "postProcessingShader", postProcessingShader });
	shaderPrograms.insert({ "pointShadowMapShader", pointShadowMapShader });
	shaderPrograms.insert({ "blurShader", blurShader });
	shaderPrograms.insert({ "pbrLightingShader", pbrLightingShader });
}

// maybe make this more efficient in the future.

Shader& ShaderManager::GetShaderProgram(std::string name) 
{
	return *shaderPrograms[name];
}

void ShaderManager::ReloadShaders() 
{
	std::cout << "Reloading Shaders...\n";

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
