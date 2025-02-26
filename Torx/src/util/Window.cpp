#define GLFW_INCLUDE_NONE

#include "Util/Window.h"

#include "../Util/ShaderManager.h"
#include "../Core/Common.h"
#include "Engine.h"
#include "../Scene/Scene.h"
#include "../Core/Coordinator.hpp"

#include <glad/glad.h>

int Window::screenWidth;
int Window::screenHeight;
double Window::m_scrollOffset{ 0 };
bool Window::cursorHidden = false;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern Coordinator ecs;

void Window::Init(int width, int height, const char* windowTitle)
{
	Window::screenWidth = width;
	Window::screenHeight = height;

	Common::SCR_WIDTH = width;
	Common::SCR_HEIGHT = height;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Full screen:
	/*const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	mWindow = glfwCreateWindow(mode->width, mode->height, windowTitle, glfwGetPrimaryMonitor(), NULL);*/

	mWindow = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);

	if (mWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(mWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// disabling vsync causes tearing when moving mouse.
	Window::EnableVsync();
	glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
	glfwSetKeyCallback(mWindow, key_callback);
}

GLFWwindow* Window::GetPointer() const{
	return mWindow;
}

void Window::Update() const {
	
	glfwPollEvents();
	glfwSwapBuffers(mWindow);
}

void Window::EnableVsync() const {
	glfwSwapInterval(1);
}
void Window::DisableVsync() const {
	glfwSwapInterval(0);
}

void Window::Terminate() const {
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

void Window::HideCursor()
{
	glfwSetInputMode(Torx::Engine::GetWindow().GetPointer(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	cursorHidden = true;
}

void Window::ShowCursor()
{
	glfwSetInputMode(Torx::Engine::GetWindow().GetPointer(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	cursorHidden = false;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		ShaderManager::ReloadShaders();
	}
}

void Window::SetScrollOffset(double offset)
{
	m_scrollOffset = offset;
}

double Window::GetScrollOffset()
{
	return m_scrollOffset;
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}