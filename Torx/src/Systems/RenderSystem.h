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
	void voxelizeScene(glm::vec3 camPos, glm::mat4 dirLightSpaceMatrix);
};