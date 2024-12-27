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
	void VoxelizeScene();
	void DirectionalShadowMapPass();
	void OmnidirectionalShadowMapPass();
	void GeometryPass();
	void SSRPass();
	void SSAOPass();
	void LightingPass();
	void SkyboxPass();
	void BloomPass();
	void PostProcessingPass();
	void ForwardRenderingPass();
	void RenderVoxelDebug();
	void RenderPhysicsDebug();
};