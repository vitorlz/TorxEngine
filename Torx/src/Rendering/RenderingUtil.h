#pragma once

#include <glm/glm.hpp>
#include <vector>

class RenderingUtil 
{
public:
	static void Init();
	
	static unsigned int mPingPongFBOs[2];
	static unsigned int mPingPongBuffers[2];
	static unsigned int mScreenQuadTexture;
	static unsigned int mBloomBrightnessTexture;
	static unsigned int mPointLightShadowMap;
	static unsigned int mDirLightShadowMap;
	static unsigned int mUnitCubeVAO;
	static unsigned int mLightingFBO;
	static unsigned int mLightingTexture;
	static unsigned int mBlittingFBO;
	static unsigned int mPointLightShadowMapFBO;
	static unsigned int mDirLightShadowMapFBO;
	static unsigned int mSSRFBO;
	static unsigned int mScreenQuadVAO;
	static unsigned int mBulletDebugLinesVAO;
	static unsigned int mBulletDebugLinesVBO;
	//static unsigned int mViewPos;
	//static unsigned int mViewNormalTexture;
	static unsigned int mRoughnessTexture;
	static unsigned int mSSRTexture;
	static unsigned int mDiffuseColorTexture;
	static unsigned int mBoxBlurFBO;
	static unsigned int mSSRBlurredTexture;
	static unsigned int mSSAONoiseTexture;
	static std::vector<glm::vec3> mSSAOKernel;
	static unsigned int mSSAOFBO;
	static unsigned int mSSAOTexture;
	static unsigned int mSSAOBlurFBO;
	static unsigned int mSSAOBlurTexture;
	static unsigned int gBufferFBO;
	static unsigned int gPosition;
	static unsigned int gNormal;
	static unsigned int gAlbedo;
	static unsigned int gRMA;
	static unsigned int gDirLightSpacePosition;
	static unsigned int gEmission;
	static unsigned int gViewPosition;
	static unsigned int gViewNormal;

	// diffuse indirect lighting
	static unsigned int mEnvironmentCubemap;
	static unsigned int mIrradianceCubemap;
	
	// specular indirect lighting
	static unsigned int mPrefilteredEnvMap;
	static unsigned int mBrdfLUT;
	
	// voxels
	static unsigned int mVoxelTexture;
	static unsigned int mVoxelizationFBO;
	static unsigned int mVoxelVisualizationTexture;

	static void EquirectangularToCubemap(const char* path);
	static void CreateIrradianceCubemap();
	static void CreatePrefilteredEnvMap();
	static void CreateBRDFIntegrationMap();
	static void CreateBulletDebugBuffers();
	static void LoadNewEnvironmentMap(const char* filename); 
	static void DeleteTexture(unsigned int textureId);
	static void CreateVoxelTexture(int voxelTextureSize);
	static void CreateSSRFBO();
	static void CreateSSRBoxBlurFBO();
	static void CreateSSAOKernel(int kernelSize);

private:
	static void CreateCubeVAO();
	static void CreateScreenQuadVAO();
	static void CreateLightingFBO();
	static void CreateBlittingFBO();
	static void CreateDirLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight);
	static void CreatePointLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight);
	static void CreatePingPongFBOs();
	static void CreateSSAONoise();
	static void CreateSSAONoiseTexture();
	static void CreateSSAOFBO();
	static void CreateSSAOBlurFBO();
	static void CreateGeometryPassFBO();
	static std::vector<glm::vec3> mSSAONoise;
};