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
};