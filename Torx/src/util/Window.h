#pragma once

#include <GLFW/glfw3.h>
#include <iostream>

struct Window 
{

	Window() = default;

	void Init(int width, int height, const char* windowTitle);
	GLFWwindow* GetPointer() const;
	void EnableVsync() const;
	void DisableVsync() const;
	void Update() const;
	void Terminate() const;
	static void SetScrollOffset(double offset);
	static double GetScrollOffset();
	static void HideCursor();
	static void ShowCursor();
	static int screenWidth;
	static int screenHeight;
	static bool cursorHidden;

private:
	static double m_scrollOffset;
	GLFWwindow* mWindow;
};