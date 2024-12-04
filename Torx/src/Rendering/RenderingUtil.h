#pragma once

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
	static unsigned int mMsFBO;
	static unsigned int mBlittingFBO;
	static unsigned int mPointLightShadowMapFBO;
	static unsigned int mDirLightShadowMapFBO;
	static unsigned int mScreenQuadVAO;
	static unsigned int mBulletDebugLinesVAO;
	static unsigned int mBulletDebugLinesVBO;

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

private:
	static void CreateCubeVAO();
	static void CreateScreenQuadVAO();
	static void CreateMSAAFBO();
	static void CreateBlittingFBO();
	static void CreateDirLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight);
	static void CreatePointLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight);
	static void CreatePingPongFBOs();
	static void CreateVoxelizationFBO();
};