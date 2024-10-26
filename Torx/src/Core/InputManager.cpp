#include "InputManager.h"
#include <unordered_map>

void InputManager::AddKey(InputKey key)
{
	keyMap[key] = true;
}

void InputManager::ResetKeys()
{
	for (const auto& pair : keyMap) {
		keyMap[pair.first] = false;
	}
}

bool InputManager::GetKey(InputKey key)
{
	return keyMap[key];
}
	
std::unordered_map<InputKey, bool> InputManager::keyMap{
	{W, false},
	{A, false},
	{S, false},
	{D, false},
	{F, false},
	{Q, false},
	{V, false},
	{SHIFT_W, false},
	{SHIFT_A, false},
	{SHIFT_S, false},
	{SHIFT_D, false},
	{SPACE, false},
	{LEFT_CONTROL, false},
	{ESC, false},
};
