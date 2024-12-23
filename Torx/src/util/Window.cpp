#include "Window.h"
#include "../Core/InputManager.h"
#include "../Util/ShaderManager.h"
#include "../UI/UI.h"
#include "../Core/Common.h"

int Window::screenWidth;
int Window::screenHeight;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

Window::Window(int width, int height, const char* windowTitle) 
{

	Window::screenWidth = width;
	Window::screenHeight = height;

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mWindow = glfwCreateWindow(screenWidth, screenHeight, windowTitle, NULL, NULL);
	if (mWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(mWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	Window::DisableVsync();
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
	glfwSetKeyCallback(mWindow, key_callback);
}

GLFWwindow* Window::GetWindow() const{
	return mWindow;
}

void Window::Update() const {
	glfwSwapBuffers(mWindow);
	glfwPollEvents();
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(mWindow, true);
	}
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (key == GLFW_KEY_TAB && action == GLFW_PRESS) 
	{
		UI::isOpen = !UI::isOpen;
		UI::firstMouseUpdateAfterMenu = true;
		if (!UI::isOpen)
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		ShaderManager::ReloadShaders();
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}