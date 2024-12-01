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
	std::shared_ptr<Shader> dirShadowMapShader = std::make_shared<Shader>("res/shaders/dirShadowMap.vert", "res/shaders/dirShadowMap.frag");
	std::shared_ptr<Shader> pointShadowMapShader = std::make_shared<Shader>("res/shaders/pointShadowMap.vert", "res/shaders/pointShadowMap.frag", "res/shaders/pointShadowMap.geom");
	std::shared_ptr<Shader> blurShader = std::make_shared<Shader>("res/shaders/gaussianBlur.vert", "res/shaders/gaussianBlur.frag");
	std::shared_ptr<Shader> pbrLightingTestShader = std::make_shared<Shader>("res/shaders/lighting.vert", "res/shaders/pbrLightingTest.frag");
	std::shared_ptr<Shader> equiToCubemapShader = std::make_shared<Shader>("res/shaders/equirectangularToCubemap.vert", "res/shaders/equirectangularToCubemap.frag");
	std::shared_ptr<Shader> irradianceConvolutionShader = std::make_shared<Shader>("res/shaders/equirectangularToCubemap.vert", "res/shaders/irradianceConvolution.frag");
	std::shared_ptr<Shader>	prefilterConvolutionShader = std::make_shared<Shader>("res/shaders/equirectangularToCubemap.vert", "res/shaders/prefilterConvolution.frag");
	std::shared_ptr<Shader>	brdfConvolutionShader = std::make_shared<Shader>("res/shaders/brdfConvolution.vert", "res/shaders/brdfConvolution.frag");
	std::shared_ptr<Shader>	pbrModelTestShader = std::make_shared<Shader>("res/shaders/lighting.vert", "res/shaders/pbrTestModelLoading.frag");
	std::shared_ptr<Shader>	lineDebugShader = std::make_shared<Shader>("res/shaders/line.vert", "res/shaders/line.frag");
	std::shared_ptr<Shader>	voxelizationShader = std::make_shared<Shader>("res/shaders/voxelization.vert", "res/shaders/voxelization.frag", "res/shaders/voxelization.geom");
	std::shared_ptr<Shader>	voxelVisualizationShader = std::make_shared<Shader>("res/shaders/voxelVisualization.vert", "res/shaders/voxelVisualization.frag");
	

	shaderPrograms.clear();

	shaderPrograms.insert({ "ourShader", ourShader });
	shaderPrograms.insert({ "cubemapShader", cubemapShader });
	shaderPrograms.insert({ "lightingShader", lightingShader });
	shaderPrograms.insert({ "solidColorShader", solidColorShader });
	shaderPrograms.insert({ "postProcessingShader", postProcessingShader });
	shaderPrograms.insert({ "pointShadowMapShader", pointShadowMapShader });
	shaderPrograms.insert({ "blurShader", blurShader });
	shaderPrograms.insert({ "pbrLightingTestShader", pbrLightingTestShader });
	shaderPrograms.insert({ "equiToCubemapShader", equiToCubemapShader });
	shaderPrograms.insert({ "irradianceConvolutionShader", irradianceConvolutionShader });
	shaderPrograms.insert({ "prefilterConvolutionShader", prefilterConvolutionShader });
	shaderPrograms.insert({ "brdfConvolutionShader", brdfConvolutionShader });
	shaderPrograms.insert({ "pbrModelTestShader", pbrModelTestShader });
	shaderPrograms.insert({ "dirShadowMapShader", dirShadowMapShader });
	shaderPrograms.insert({ "lineDebugShader", lineDebugShader });
	shaderPrograms.insert({ "voxelizationShader", voxelizationShader });
	shaderPrograms.insert({ "voxelVisualizationShader", voxelVisualizationShader });
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
