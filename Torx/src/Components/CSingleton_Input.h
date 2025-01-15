#pragma once

#include <unordered_map>
#include "../Core/Common.h"
#include "../Util/Keys.h"

class CSingleton_Input
{
public:
	static CSingleton_Input& getInstance()
	{
		static CSingleton_Input instance;
		
		return instance;
	}

private:
	CSingleton_Input() {};

public:
	CSingleton_Input(CSingleton_Input const&) = delete;
	void operator = (CSingleton_Input const&) = delete;

	bool keyPressed[349];
	bool keyDown[349];
	bool leftMousePressed;
	bool leftMouseDown;
	bool rightMousePressed;
	bool rightMouseDown;
	bool middleMousePressed;
	bool middleMouseDown;
	bool firstMouse;
	double mouseOffsetX;
	double mouseOffsetY;
	double mouseX;
	double mouseY;

};