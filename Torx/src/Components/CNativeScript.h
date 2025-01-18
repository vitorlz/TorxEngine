#pragma once 

#include <functional>
#include <unordered_map>
#include <iostream>
#include "../Core/Types.hpp"

class ScriptableEntity
{
public:
	virtual void onUpdate(float dt) {};
	virtual void onCreate() {};
	virtual void onDestroy() {};

	virtual ~ScriptableEntity() = default;

	Entity m_entity;
};

struct CNativeScript
{
	std::string name;
	ScriptableEntity* script = nullptr;
};

// We are going to have a script factory which will handle the instantiation of scripts for us at runtime.
// This is the way it is going to work: the scriptFactory will have a Create() and a Register() function. The Register() function
// will be used to insert a pair in a map containing the name of the script (eg. CameraController) and a std::function which will hold a function that
// actually allocates memory for the script on the heap and return a pointer to it. The create function will simply call that function if the
// script name given to it as a parameter exists in the map. 

class ScriptFactory
{
public:
	// insert a name and function pair to the map
	static void Register(const std::string& name, std::function<ScriptableEntity*()> createFunction)
	{
		m_registry.insert({ name, createFunction });

		std::cout << "Registered script" << name << "\n";
	}

	// actually create the pointer to the scriptable entity and return it using the stored name and funtion when we need it.
	static ScriptableEntity* Create(const std::string& name)
	{
		auto it = m_registry.find(name);

		if (it != m_registry.end())
		{
			return it->second();
		}
		
		std::cout << "Script not registered" << "\n";
		return nullptr;
	}

	static std::vector<std::string> GetNames()
	{
		std::vector<std::string> names;
		for (auto& pair : m_registry)
		{
			names.push_back(pair.first);
		}

		return names;
	}
private:
	static inline std::unordered_map <std::string, std::function<ScriptableEntity*()>> m_registry;
};