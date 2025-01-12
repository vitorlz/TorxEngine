#include "Window.h"

#include "../Util/ShaderManager.h"
#include "../UI/UI.h"
#include "../Core/Common.h"
#include "../include/Engine.h"
#include "../Editor/EditorCamera.h"

#include <glad/glad.h>

int Window::screenWidth;
int Window::screenHeight;
double Window::m_scrollOffset{ 0 };

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void Window::Init(int width, int height, const char* windowTitle)
{
	Window::screenWidth = width;
	Window::screenHeight = height;

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
	Window::DisableVsync();
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetFramebufferSizeCallback(mWindow, framebuffer_size_callback);
	glfwSetKeyCallback(mWindow, key_callback);
	glfwSetScrollCallback(mWindow, scroll_callback);
	
}

GLFWwindow* Window::GetPointer() const{
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

		if (Torx::Engine::MODE == Torx::EDITOR)
		{
			Torx::Engine::MODE = Torx::PLAY;
		}
		else
		{
			Torx::Engine::MODE = Torx::EDITOR;

		}
		
		UI::firstMouseUpdateAfterMenu = true;
		if (Torx::Engine::MODE == Torx::PLAY)
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

void Window::SetScrollOffset(double offset)
{
	m_scrollOffset = offset;
}

double Window::GetScrollOffset()
{
	return m_scrollOffset;
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	Window::SetScrollOffset(yoffset);

	EditorCamera& editorCamera = EditorCamera::getInstance();

	static float fov = 45.0f;
	
	fov -= (float)yoffset;
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 90.0f)
		fov = 90.0f;

	editorCamera.SetFov(fov);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}