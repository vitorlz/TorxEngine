#pragma once

class RenderingUtil 
{
public:
	static unsigned int CreateCubeVAO();
	static unsigned int CreateScreenQuadVAO();
	static unsigned int CreateMSAAFBO();
	static unsigned int CreateBlittingFBO();

	static unsigned int GetScreenQuadTexture();
	static unsigned int GetBloomBrightnessTexture();

private:
	static unsigned int mScreenQuadTexture;
	static unsigned int mBloomBrightnessTexture;
};