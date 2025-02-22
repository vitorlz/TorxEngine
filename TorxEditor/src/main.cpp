#include <Core/Common.h>
#include <Engine.h>
#include "Editor/Editor.h"
#include "Engine.h"
#include "Core/Common.h"
#include "Core/Coordinator.hpp"

#include "Util/ShaderManager.h"
#include "AssetLoading/AssetManager.h"
#include "Rendering/TextRendering.h"
#include "Scene/Scene.h"
#include "Components/CNativeScript.h"
#include "Components/CTransform.h"
#include "Game/PlayerInputScript.hpp"
#include "Game/TestController.hpp"

int main()
{
    Torx::Engine& engine = Torx::Engine::getInstance();
    engine.ConfigWindow(1600, 900, "TorxEditor");
    engine.Init();

    AssetManager::LoadModel("res/models/zombie/zombie.gltf", "zombie");

    AssetManager::LoadAssets();

    ScriptFactory::Register("PlayerController", []() { return new PlayerController(); });
    ScriptFactory::Register("TestController", []() { return new TestController(); });

    Torx::Engine::MODE = Torx::EDITOR;
    Editor& editor = Editor::getInstance();
    editor.InitUI();

    editor.Run();

    return 0;
}