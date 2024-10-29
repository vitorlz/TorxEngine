#pragma once 

#include "../Core/System.hpp"

class LightSystem : public System
{
public:
	void Init();

	void Update(float deltaTime);

private:
	unsigned int mSsbo;
	int mMaxLights;
	int mLightNumber;
};