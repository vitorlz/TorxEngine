#pragma once 

#include "../Core/System.hpp"

class LightSystem : public System
{
public:
	void Init();

	void Update(float deltaTime);
	unsigned int mSsbo;
private:
	int mMaxLights;
	int mLightIndex;
};