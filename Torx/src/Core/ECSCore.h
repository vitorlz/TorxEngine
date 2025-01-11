#pragma once 


#include <vector>
#include "../Core/System.hpp"
#include <memory>

class ECSCore
{
public:
	
	static void RegisterCoreComponentsAndSystems();
	static void UpdateSystems(float dt);
	static void InitSystems();

private:
	static std::vector<std::shared_ptr<System>> m_systems;

};