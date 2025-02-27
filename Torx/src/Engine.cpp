#include "Engine.h"
#include "Core/Common.h"
#include "Core/Coordinator.hpp"

#include "Util/ShaderManager.h"
#include "AssetLoading/AssetManager.h"
#include "Util/RenderingUtil.h"
#include "Misc/TextRendering.h"
#include "Scene/Scene.h"
#include "Components/CNativeScript.h"
#include "Components/CTransform.h"
#include "Game/PlayerInputScript.hpp"
#include "Game/TestController.hpp"

Coordinator ecs;

using namespace Torx;

Window Engine::m_window;

EngineMode Engine::MODE{ Torx::PLAY };

Engine::Engine() {};

Engine& Engine::getInstance()
{
	static Engine instance;
	return instance;
}

void Engine::ConfigWindow(int width, int height, const char* title) 
{
	m_window.Init(width, height, title);
}

Window& Engine::GetWindow()
{
	return m_window;
}

void Engine::Init()
{
	TextRendering::Init();
	ShaderManager::LoadShaders();
	RenderingUtil::Init();
	ECSCore::RegisterCoreComponentsAndSystems();
	ECSCore::InitSystems();
}

void Engine::Run()
{
	float deltaTime{};
	float lastFrame{};

	while (!glfwWindowShouldClose(GetWindow().GetPointer()))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		ECSCore::UpdateSystems(deltaTime);

		GetWindow().Update();
	}

	FT_Done_FreeType(TextRendering::m_ft);
	GetWindow().Terminate();  
}



