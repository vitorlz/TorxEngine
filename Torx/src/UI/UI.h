#pragma once
#include <GLFW/glfw3.h>

class UI 
{
public:
	void Init(GLFWwindow* window);
	void NewFrame();
	void Update();
	void Terminate();
};