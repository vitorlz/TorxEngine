#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Core/Types.hpp"

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
	static bool spectatingCamera;

	void RenderGameWindow();

	void showComponents(Entity entity);

	static bool projectLoaded;
	std::string projectsDir = "";

	static glm::vec2 gameWindowMousePos;
	static glm::vec2 gameWindowSize;
	static glm::vec2 gameWindowPos;
};