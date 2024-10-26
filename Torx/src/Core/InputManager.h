#pragma once

#include "Types.hpp"
#include <vector>
#include <iostream>
#include <unordered_map>

class InputManager
{
public:
	static void AddKey(InputKey key);
	static void ResetKeys();
	static bool GetKey(InputKey key);

private:
	static std::unordered_map<InputKey, bool> keyMap;
};
