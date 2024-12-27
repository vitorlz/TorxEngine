#pragma once 

// Store shadow state and data

namespace OmniShadows
{
	inline int g_shadowCasterCount{ 0 };
};

namespace DirectionalShadows
{
	inline glm::mat4 g_lightSpaceMatrix;
}