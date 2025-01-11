#pragma once

#include "Types.hpp"
#include <set>

class System
{
public:
	std::set<Entity> mEntities;
	virtual void Update(float dt) = 0;
	virtual void Init() = 0;
};