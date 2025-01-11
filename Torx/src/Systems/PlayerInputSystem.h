#pragma once 

#include "../Core/System.hpp"


class PlayerInputSystem : public System
{
public:
	void Init() override;

	void Update(float deltaTime) override;
};