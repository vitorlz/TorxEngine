#include "ShaderManager.h"
#include "memory"
#include <vector>
#include <glad/glad.h>

std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderManager::shaderPrograms;

void ShaderManager::LoadShaders() 
{
	std::shared_ptr<Shader> cubemapShader = std::make_shared<Shader>("res/shaders/cubemap.vert", "res/shaders/cubemap.frag");
	std::shared_ptr<Shader> solidColorShader = std::make_shared<Shader>("res/shaders/solidColor.vert", "res/shaders/solidColor.frag");
	std::shared_ptr<Shader> postProcessingShader = std::make_shared<Shader>("res/shaders/postProcessing.vert", "res/shaders/postProcessing.frag");
	std::shared_ptr<Shader> dirShadowMapShader = std::make_shared<Shader>("res/shaders/dirShadowMap.vert", "res/shaders/dirShadowMap.frag");
	std::shared_ptr<Shader> pointShadowMapShader = std::make_shared<Shader>("res/shaders/pointShadowMap.vert", "res/shaders/pointShadowMap.frag", "res/shaders/pointShadowMap.geom");
	std::shared_ptr<Shader> blurShader = std::make_shared<Shader>("res/shaders/gaussianBlur.vert", "res/shaders/gaussianBlur.frag");
	std::shared_ptr<Shader> equiToCubemapShader = std::make_shared<Shader>("res/shaders/equirectangularToCubemap.vert", "res/shaders/equirectangularToCubemap.frag");
	std::shared_ptr<Shader> irradianceConvolutionShader = std::make_shared<Shader>("res/shaders/equirectangularToCubemap.vert", "res/shaders/irradianceConvolution.frag");
	std::shared_ptr<Shader>	prefilterConvolutionShader = std::make_shared<Shader>("res/shaders/equirectangularToCubemap.vert", "res/shaders/prefilterConvolution.frag");
	std::shared_ptr<Shader>	brdfConvolutionShader = std::make_shared<Shader>("res/shaders/brdfConvolution.vert", "res/shaders/brdfConvolution.frag");
	std::shared_ptr<Shader>	lineDebugShader = std::make_shared<Shader>("res/shaders/line.vert", "res/shaders/line.frag");
	std::shared_ptr<Shader>	voxelizationShader = std::make_shared<Shader>("res/shaders/voxelization.vert", "res/shaders/voxelization.frag", "res/shaders/voxelization.geom");
	std::shared_ptr<Shader>	voxelVisualizationShader = std::make_shared<Shader>("res/shaders/voxelVisualization.vert", "res/shaders/voxelVisualization.frag", "res/shaders/voxelVisualization.geom");
	std::shared_ptr<Shader>	lightingShader = std::make_shared<Shader>("res/shaders/lighting.vert", "res/shaders/lighting.frag");
	std::shared_ptr<Shader>	ssrShader = std::make_shared<Shader>("res/shaders/SSR.vert", "res/shaders/SSR.frag");
	std::shared_ptr<Shader>	boxBlurShader = std::make_shared<Shader>("res/shaders/boxBlur.vert", "res/shaders/boxBlur.frag");
	std::shared_ptr<Shader>	ssaoShader = std::make_shared<Shader>("res/shaders/SSAO.vert", "res/shaders/SSAO.frag");
	std::shared_ptr<Shader>	ssaoBlurShader = std::make_shared<Shader>("res/shaders/SSAOBlur.vert", "res/shaders/SSAOBlur.frag");
	std::shared_ptr<Shader>	gBufferShader = std::make_shared<Shader>("res/shaders/gBufferShader.vert", "res/shaders/gBufferShader.frag");
	std::shared_ptr<Shader>	animShader = std::make_shared<Shader>("res/shaders/anim.vert", "res/shaders/anim.frag");
	std::shared_ptr<Shader>	textShader = std::make_shared<Shader>("res/shaders/text.vert", "res/shaders/text.frag");
	std::shared_ptr<Shader>	iconShader = std::make_shared<Shader>("res/shaders/icon.vert", "res/shaders/text.frag");

	shaderPrograms.clear();

	shaderPrograms.insert({ "cubemapShader", cubemapShader });
	shaderPrograms.insert({ "lightingShader", lightingShader });
	shaderPrograms.insert({ "solidColorShader", solidColorShader });
	shaderPrograms.insert({ "postProcessingShader", postProcessingShader });
	shaderPrograms.insert({ "pointShadowMapShader", pointShadowMapShader });
	shaderPrograms.insert({ "blurShader", blurShader });
	shaderPrograms.insert({ "equiToCubemapShader", equiToCubemapShader });
	shaderPrograms.insert({ "irradianceConvolutionShader", irradianceConvolutionShader });
	shaderPrograms.insert({ "prefilterConvolutionShader", prefilterConvolutionShader });
	shaderPrograms.insert({ "brdfConvolutionShader", brdfConvolutionShader });
	shaderPrograms.insert({ "dirShadowMapShader", dirShadowMapShader });
	shaderPrograms.insert({ "lineDebugShader", lineDebugShader });
	shaderPrograms.insert({ "voxelizationShader", voxelizationShader });
	shaderPrograms.insert({ "voxelVisualizationShader", voxelVisualizationShader });
	shaderPrograms.insert({ "lightingShader", lightingShader });
	shaderPrograms.insert({ "ssrShader", ssrShader });
	shaderPrograms.insert({ "boxBlurShader", boxBlurShader });
	shaderPrograms.insert({ "ssaoShader", ssaoShader });
	shaderPrograms.insert({ "ssaoBlurShader", ssaoBlurShader });
	shaderPrograms.insert({ "gBufferShader", gBufferShader });
	shaderPrograms.insert({ "animShader", animShader });
	shaderPrograms.insert({ "textShader", textShader });
	shaderPrograms.insert({ "iconShader", iconShader });
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
