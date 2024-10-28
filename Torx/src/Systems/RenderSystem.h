#pragma once

#include "../Core/System.hpp"
#include "../Util/Shader.h"
#include <unordered_map>
#include "../Util/Camera.h"
#include "iostream"


class RenderSystem : public System
{
public:
	void Init();

	void Update(float deltaTime, Camera& camera);
private:
	//std::unordered_map<const char*, Shader> mShaders;
	Camera mCamera;
	unsigned int mCubemapID;
	unsigned int mCubeVAO;
};