#include "Window.h"
#include "../Core/InputManager.h"

int Window::screenWidth;
int Window::screenHeight;

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
	glfwSwapInterval(1);
	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	menu = false;
}

GLFWwindow* Window::GetWindow() const{
	return mWindow;
}

void Window::Update() const {
	glfwSwapBuffers(mWindow);
	glfwPollEvents();
	if (InputManager::GetKey(ESC)) {
		glfwSetWindowShouldClose(mWindow, true);
	}
	InputManager::ResetKeys();
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


void Window::ProcessInputs() {
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		InputManager::AddKey(ESC);
	if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
		InputManager::AddKey(W);
	if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
		InputManager::AddKey(S);
	if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
		InputManager::AddKey(A);
	if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
		InputManager::AddKey(D);
	if (glfwGetKey(mWindow, GLFW_KEY_V) == GLFW_PRESS)
		InputManager::AddKey(V);
	if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
		InputManager::AddKey(SPACE);
	if (glfwGetKey(mWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		InputManager::AddKey(LEFT_CONTROL);
	if (glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		InputManager::AddKey(SHIFT_W);
	if (glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		InputManager::AddKey(SHIFT_S);
	if (glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		InputManager::AddKey(SHIFT_A);
	if (glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS && glfwGetKey(mWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		InputManager::AddKey(SHIFT_D);	
	if (glfwGetKey(mWindow, GLFW_KEY_TAB) == GLFW_PRESS)
		InputManager::AddKey(TAB);
}
