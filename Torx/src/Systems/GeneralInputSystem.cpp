#include "GeneralInputSystem.h"
//#include "../Util/Window.h"

#include "../Core/Coordinator.hpp"
#include "../Components/CSingleton_Input.h"
#include "../Core/Common.h"
#include <iostream>


extern Coordinator ecs;

void GeneralInputSystem::Update(float deltaTime, GLFWwindow* window)
{	

	CSingleton_Input& inputSing = CSingleton_Input::getInstance();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) 
	{
		inputSing.pressedKeys[ESC] = true;
	}		
	else if ((glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[ESC] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		inputSing.pressedKeys[W] = true;
	}	
	else if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[W] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		inputSing.pressedKeys[S] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[S] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		inputSing.pressedKeys[A] = true;
	}	
	else if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[A] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		inputSing.pressedKeys[D] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[D] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
	{
		inputSing.pressedKeys[V] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[V] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		inputSing.pressedKeys[K] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[K] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		inputSing.pressedKeys[SPACE] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[SPACE] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		inputSing.pressedKeys[LEFT_CONTROL] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[LEFT_CONTROL] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		inputSing.pressedKeys[SHIFT_W] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[SHIFT_W] = false;
	}	
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		inputSing.pressedKeys[SHIFT_S] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[SHIFT_S] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		inputSing.pressedKeys[SHIFT_A] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[SHIFT_A] = false;
	}	
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
	{
		inputSing.pressedKeys[SHIFT_D] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE || glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[SHIFT_D] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
	{
		inputSing.pressedKeys[TAB] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[TAB] = false;
	}
}

