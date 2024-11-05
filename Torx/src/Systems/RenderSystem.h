#pragma once

#include "../Core/System.hpp"
#include "../Util/Shader.h"
#include <unordered_map>
#include "iostream"
#include "../Util/Shader.h"


class RenderSystem : public System
{
public:
	void Init();

	void Update(float deltaTime);
private:
	unsigned int mCubemapID;
	unsigned int mCubeVAO;
	unsigned int mScreenQuadVAO;
	unsigned int mMsFBO;
	unsigned int mBlittingFBO;
	unsigned int mScreenQuadTexture;
	unsigned int mPointLightShadowMapFBO;
	unsigned int mPointLightShadowMap;
};