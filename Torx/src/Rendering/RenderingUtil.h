#pragma once

class RenderingUtil 
{
public:
	static unsigned int CreateCubeVAO();
	static unsigned int CreateScreenQuadVAO();
	static unsigned int CreateMSAAFBO();
	static unsigned int CreateBlittingFBO();
	static unsigned int CreatePointLightShadowMapFBO(unsigned int shadowWidth, unsigned int shadowHeight);
	static void CreatePingPongFBOs();

	static unsigned int GetScreenQuadTexture();
	static unsigned int GetBloomBrightnessTexture();
	static unsigned int GetPointLightShadowMap();
	
	static unsigned int mPingPongFBOs[2];
	static unsigned int mPingPongBuffers[2];

private:
	static unsigned int mScreenQuadTexture;
	static unsigned int mBloomBrightnessTexture;
	static unsigned int mPointLightShadowMap;
};