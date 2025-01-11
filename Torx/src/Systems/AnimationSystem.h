#pragma once 

#include "../Core/System.hpp"


class AnimationSystem : public System
{
public:
	void Init() override;

	void Update(float deltaTime) override;
};