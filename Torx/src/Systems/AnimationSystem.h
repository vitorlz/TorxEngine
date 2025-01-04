#pragma once 

#include "../Core/System.hpp"


class AnimationSystem : public System
{
public:
	void Init();

	void Update(float deltaTime);
};