#include "../include/Engine.h"
#include "Core/Common.h"
#include "Core/Coordinator.hpp"

#include "Util/ShaderManager.h"
#include "AssetLoading/AssetManager.h"
#include "Rendering/RenderingUtil.h"
#include "Rendering/TextRendering.h"
#include "Scene/Scene.h"

Coordinator ecs;

using namespace Torx;

Window Engine::m_window;

EngineMode Engine::MODE{ EDITOR };

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

void Engine::Init(const std::string scenePath)
{
	TextRendering::Init();
	AssetManager::LoadAssets();
	ShaderManager::LoadShaders();
	RenderingUtil::Init();
	ECSCore::RegisterCoreComponentsAndSystems();
	Scene::LoadSceneFromJson(scenePath);
	ECSCore::InitSystems();
}

void Engine::Init()
{
	TextRendering::Init();
	AssetManager::LoadAssets();
	ShaderManager::LoadShaders();
	RenderingUtil::Init();
	ECSCore::RegisterCoreComponentsAndSystems();
	ECSCore::InitSystems();
}

void Engine::Run(float deltaTime)
{
	ECSCore::UpdateSystems(deltaTime);
}



