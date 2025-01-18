#pragma once 

#include "../Core/System.hpp"


class NativeScriptSystem : public System
{
public:
	void Init() override;

	void Update(float deltaTime) override;
};