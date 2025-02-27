#pragma once 

#include "glm/glm.hpp"

// Store shadow state and data

namespace OmniShadows
{
	inline int g_shadowCasterCount{ 0 };
	inline int g_resolution{ 1024 };
};

namespace DirectionalShadows
{
	inline glm::mat4 g_lightSpaceMatrix;
	inline int g_resolution{ 2048 };
}