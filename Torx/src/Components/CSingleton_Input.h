#pragma once

#include <unordered_map>
#include "../Core/Common.h"

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

	std::unordered_map<InputKey, bool> pressedKeys;

};