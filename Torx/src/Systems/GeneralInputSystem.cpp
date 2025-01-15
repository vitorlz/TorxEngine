#include "GeneralInputSystem.h"
//#include "../Util/Window.h"

#include "../Core/Coordinator.hpp"
#include "../Components/CSingleton_Input.h"
#include "../Core/Common.h"
#include "../UI/UI.h"
#include <iostream>
#include "../include/Engine.h"
#include "../Util/Keys.h"

extern Coordinator ecs;

static GLFWwindow* window{};


void GeneralInputSystem::Init() 
{
	CSingleton_Input& inputSing = CSingleton_Input::getInstance();

	window = Torx::Engine::GetWindow().GetPointer();

	inputSing.firstMouse = true;
}

void GeneralInputSystem::Update(float deltaTime)
{	
	CSingleton_Input& inputSing = CSingleton_Input::getInstance();

	// keyboard input
	static bool keyDownLastFrame[349];

	for (int i = 32; i < 349; i++)
	{
		if (glfwGetKey(window, i) == GLFW_PRESS)
			inputSing.keyDown[i] = true;
		else
			inputSing.keyDown[i] = false;

		if (inputSing.keyDown[i] && !keyDownLastFrame[i])
			inputSing.keyPressed[i] = true;
		else
			inputSing.keyPressed[i] = false;

		keyDownLastFrame[i] = inputSing.keyDown[i];
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

	inputSing.leftMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
	inputSing.rightMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;
	inputSing.middleMouseDown = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS;

	static bool leftMouseDownLastFrame = false;
	static bool rightMouseDownLastFrame = false;
	static bool middleMouseDownLastFrame = false;

	if (inputSing.leftMouseDown && !leftMouseDownLastFrame)
		inputSing.leftMousePressed = true;
	else
		inputSing.leftMousePressed = false;
	
	if (inputSing.rightMouseDown && !rightMouseDownLastFrame)
		inputSing.rightMousePressed = true;
	else
		inputSing.rightMousePressed = false;
	
	if (inputSing.middleMouseDown && !middleMouseDownLastFrame)
		inputSing.middleMousePressed = true;
	else
		inputSing.middleMousePressed = false;
	
	leftMouseDownLastFrame = inputSing.leftMouseDown;
	rightMouseDownLastFrame = inputSing.rightMouseDown;
	middleMouseDownLastFrame = inputSing.middleMouseDown;
}

