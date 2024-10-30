#pragma once 

#include "../Util/Camera.h"
#include "../Core/System.hpp"

class LightSystem : public System
{
public:
	void Init();

	void Update(float deltaTime, Camera& camera);

private:
	unsigned int mSsbo;
	int mMaxLights;
	int mLightIndex;
};