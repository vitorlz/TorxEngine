#pragma once 

#include "../Core/System.hpp"
#include <GLFW/glfw3.h>

class GeneralInputSystem : public System
{
public:
	void Init() override;

	void Update(float deltaTime) override;
};