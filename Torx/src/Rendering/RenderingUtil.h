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
	static unsigned int mEnvironmentCubemap;
	static unsigned int mIrradianceCubemap;

	static void EquirectangularToCubemap(const char* path);
	static void CreateIrradianceCubemap();

private:
	static void CreateCubeVAO();
	static void CreateScreenQuadVAO();
	static void CreateMSAAFBO();
	static void CreateBlittingFBO();
	static void CreatePointLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight);
	static void CreatePingPongFBOs();
};