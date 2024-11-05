#pragma once 

#include "../Core/System.hpp"


class PlayerInputSystem : public System
{
public:
	void Init();

	void Update(float deltaTime);
};