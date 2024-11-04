#pragma once 

#include "../Core/System.hpp"
#include <GLFW/glfw3.h>

class GeneralInputSystem : public System
{
public:
	void Init();

	void Update(float deltaTime, GLFWwindow* window);
};