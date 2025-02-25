#include <Engine.h>

#include <AssetLoading/AssetManager.h>
#include <Game/PlayerInputScript.hpp>
#include <Game/TestController.hpp>
#include <Scene/Scene.h>

int main()
{
    Torx::Engine& engine = Torx::Engine::getInstance();
    engine.ConfigWindow(1600, 900, "Sandbox");
    engine.Init();

    AssetManager::LoadModel("res/models/zombie/zombie.gltf", "zombie");
    AssetManager::LoadAssets();

    ScriptFactory::Register("PlayerController", []() { return new PlayerController(); });
    ScriptFactory::Register("TestController", []() { return new TestController(); });

    Scene::LoadSceneFromJson("res/scenes/sponzascene4.json");

    engine.Run();
}