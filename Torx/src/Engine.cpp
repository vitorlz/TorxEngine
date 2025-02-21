#include "Engine.h"
#include "Core/Common.h"
#include "Core/Coordinator.hpp"

#include "Util/ShaderManager.h"
#include "AssetLoading/AssetManager.h"
#include "Rendering/RenderingUtil.h"
#include "Rendering/TextRendering.h"
#include "Scene/Scene.h"
#include "Components/CNativeScript.h"
#include "Components/CTransform.h"
#include "Game/PlayerInputScript.hpp"
#include "Game/TestController.hpp"
#include "Editor/Editor.h"

Coordinator ecs;

using namespace Torx;

Window Engine::m_window;

EngineMode Engine::MODE{};

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

void Engine::Run()
{

    ScriptFactory::Register("PlayerController", []() { return new PlayerController(); });
    ScriptFactory::Register("TestController", []() { return new TestController(); });

#ifdef ENGINE_MODE_EDITOR
    Torx::Engine::MODE = Torx::EDITOR;
    Editor& editor = Editor::getInstance();
    editor.InitUI();
#else
    Torx::Engine::MODE = Torx::PLAY;
#endif 

#ifdef ENGINE_MODE_GAME
    Scene::LoadSceneFromJson("res/scenes/sponzascene2.json");
#endif 

    float deltaTime{};
    float lastFrame{};

    while (!glfwWindowShouldClose(GetWindow().GetPointer()))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        ECSCore::UpdateSystems(deltaTime);

#ifdef ENGINE_MODE_EDITOR
        editor.Update(deltaTime);
#endif 

        GetWindow().Update();
    }

#ifdef ENGINE_MODE_EDITOR
    editor.GetUI().Terminate();
#endif 

    FT_Done_FreeType(TextRendering::m_ft);
    GetWindow().Terminate();
}



