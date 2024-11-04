#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

struct Window 
{
	Window(int width, int height, const char* windowTitle);

	GLFWwindow* GetWindow() const;
	void EnableVsync() const;
	void DisableVsync() const;
	void Update() const;
	void Terminate() const;
	static int screenWidth;
	static int screenHeight;

private:
	GLFWwindow* mWindow;
};