#include "Window.h"

Window::Window(int width, int height, const char* windowTitle) 
	:mWidth(width), mHeight(height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	mWindow = glfwCreateWindow(mWidth, mHeight, windowTitle, NULL, NULL);
	if (mWindow == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}

	glfwMakeContextCurrent(mWindow);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);
}

GLFWwindow* Window::GetWindow() const{
	return mWindow;
}

void Window::Update() const {
	glfwSwapBuffers(mWindow);
	glfwPollEvents();
}

void Window::EnableVsync() const {
	glfwSwapInterval(1);
}
void Window::DisableVsync() const {
	glfwSwapInterval(1);
}

void Window::Terminate() const {
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}


void Window::ProcessInputs() const {
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(mWindow, true);
}

