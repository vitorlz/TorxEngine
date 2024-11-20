#include "GeneralInputSystem.h"
//#include "../Util/Window.h"

#include "../Core/Coordinator.hpp"
#include "../Components/CSingleton_Input.h"
#include "../Core/Common.h"
#include "../UI/UI.h"
#include <iostream>



extern Coordinator ecs;

void GeneralInputSystem::Init() 
{
	CSingleton_Input& inputSing = CSingleton_Input::getInstance();

	inputSing.firstMouse = true;
}

void GeneralInputSystem::Update(float deltaTime, GLFWwindow* window)
{	
	CSingleton_Input& inputSing = CSingleton_Input::getInstance();

	// Key input

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
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		inputSing.pressedKeys[F] = true;
	}
	else if ((glfwGetKey(window, GLFW_KEY_K) == GLFW_RELEASE))
	{
		inputSing.pressedKeys[F] = false;
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
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		inputSing.pressedKeys[MOUSE_LEFT] = true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		inputSing.pressedKeys[MOUSE_LEFT] = false;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		inputSing.pressedKeys[MOUSE_RIGHT] = true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_RELEASE)
	{
		inputSing.pressedKeys[MOUSE_RIGHT] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		std::cout << "1 pressed \n";
		inputSing.pressedKeys[NUMBER_1] = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
	{
		inputSing.pressedKeys[NUMBER_1] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		inputSing.pressedKeys[NUMBER_2] = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
	{
		inputSing.pressedKeys[NUMBER_2] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		inputSing.pressedKeys[NUMBER_3] = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE)
	{
		inputSing.pressedKeys[NUMBER_3] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		inputSing.pressedKeys[NUMBER_4] = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE)
	{
		inputSing.pressedKeys[NUMBER_4] = false;
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		inputSing.pressedKeys[NUMBER_5] = true;
	}
	else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE)
	{
		inputSing.pressedKeys[NUMBER_5] = false;
	}

	// mouse input

	double x, y;
	glfwGetCursorPos(window, &x, &y);
	if (inputSing.firstMouse || UI::firstMouseUpdateAfterMenu) {
		inputSing.mouseX = x;
		inputSing.mouseY = y;
		inputSing.firstMouse = false;
		UI::firstMouseUpdateAfterMenu = false;
	}

	inputSing.mouseOffsetX = x - inputSing.mouseX;
	inputSing.mouseOffsetY = y - inputSing.mouseY;
	inputSing.mouseX = x;
	inputSing.mouseY = y;
}

