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
	static unsigned int mUnitCubeVAO;
	static unsigned int mMsFBO;
	static unsigned int mBlittingFBO;
	static unsigned int mPointLightShadowMapFBO;
	static unsigned int mScreenQuadVAO;
	// diffuse indirect lighting
	static unsigned int mEnvironmentCubemap;
	static unsigned int mIrradianceCubemap;
	// specular indirect lighting
	static unsigned int mPrefilteredEnvMap;
	static unsigned int mBrdfLUT;

	static void EquirectangularToCubemap(const char* path);
	static void CreateIrradianceCubemap();
	static void CreatePrefilteredEnvMap();
	static void CreateBRDFIntegrationMap();

private:
	static void CreateCubeVAO();
	static void CreateScreenQuadVAO();
	static void CreateMSAAFBO();
	static void CreateBlittingFBO();
	static void CreatePointLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight);
	static void CreatePingPongFBOs();
};