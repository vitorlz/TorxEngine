#pragma once

#include "Types.hpp"
#include <vector>
#include <iostream>
#include <unordered_map>
#include "../Core/Common.h"

class InputManager
{
public:
	static void AddKey(InputKey key);
	static void ResetKeys();
	static bool GetKey(InputKey key);

	static std::unordered_map<InputKey, bool> keyMap;
};
