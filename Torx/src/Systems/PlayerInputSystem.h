#pragma once 

#include "../Core/System.hpp"
#include "../Util/Camera.h"

class PlayerInputSystem : public System
{
public:
	void Init();

	void Update(float deltaTime, Camera& camera);
};