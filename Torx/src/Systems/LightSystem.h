#pragma once 

#include "../Core/System.hpp"

class LightSystem : public System
{
public:
	void Init() override;

	void Update(float deltaTime) override;
	unsigned int mSsbo;
private:
	int mMaxLights;
	int mLightIndex;
};