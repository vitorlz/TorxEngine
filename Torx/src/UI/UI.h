#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class UI 
{
public:
	void Init(GLFWwindow* window);
	void NewFrame();
	void Update();
	void Terminate();
	static bool isOpen;
	static bool firstMouseUpdateAfterMenu;
	static bool hovering;

	void RenderGameWindow();

	static glm::vec2 gameWindowMousePos;
	static glm::vec2 gameWindowSize;
	static glm::vec2 gameWindowPos;
};