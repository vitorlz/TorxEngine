#pragma once

#include "../Core/System.hpp"
#include "../Util/Shader.h"
#include <unordered_map>
#include "iostream"
#include "../Util/Shader.h"


class RenderSystem : public System
{
public:
	void Init() override;

	void Update(float deltaTime) override;

private:
	void voxelizationPass();
	void directionalShadowMapPass();
	void omnidirectionalShadowMapPass();
	void geometryPass();
	void ssrPass();
	void ssaoPass();
	void lightingPass();
	void skyboxPass();
	void bloomPass();
	void postProcessingPass();
	void forwardRenderingPass();
	void renderVoxelDebug();
	void renderPhysicsDebug();
};