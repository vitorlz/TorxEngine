#pragma once 

#include "../Util/Camera.h"
#include "../Core/System.hpp"

class LightSystem : public System
{
public:
	void Init();

	void Update(float deltaTime, Camera& camera);
	unsigned int mSsbo;
private:
	int mMaxLights;
	int mLightIndex;
};