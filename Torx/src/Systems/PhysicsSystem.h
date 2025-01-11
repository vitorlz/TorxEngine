#pragma once 

#include "../Core/System.hpp"


class PhysicsSystem : public System
{
public:
	void Init() override;

	void Update(float deltaTime) override;
};