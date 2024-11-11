#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "UI/UI.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Common.h"
#include "Util/Shader.h"
#include "Util/Window.h"
#include "Core/Coordinator.hpp"
#include "Util/ShaderManager.h"
#include "AssetLoading/AssetManager.h"

#include "Components/CTransform.h"
#include "Components/CLight.h"
#include "Components/CModel.h"
#include "Components/CSingleton_Input.h"
#include "Components/CPlayer.h"

#include "Systems/RenderSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/GeneralInputSystem.h"
#include "Systems/playerInputSystem.h"

#include <iostream>

// settings

Coordinator ecs;

int main()
{
    ecs.Init();

    Window window(Common::SCR_WIDTH, Common::SCR_HEIGHT, "Torx");

    window.DisableVsync();

    float deltaTime{};
    float lastFrame{};

    AssetManager::LoadModels();
    ShaderManager::LoadShaders();

    ecs.RegisterComponent<CTransform>();
    ecs.RegisterComponent<CModel>();
    ecs.RegisterComponent<CLight>();
    ecs.RegisterComponent<CPlayer>();

    auto renderSystem = ecs.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        signature.set(ecs.GetComponentType<CModel>());
        ecs.SetSystemSignature<RenderSystem>(signature);
    }

    auto lightSystem = ecs.RegisterSystem<LightSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        signature.set(ecs.GetComponentType<CLight>());
        ecs.SetSystemSignature<LightSystem>(signature);
    }

    auto generalInputSystem = ecs.RegisterSystem<GeneralInputSystem>();
    {
        Signature signature;
        ecs.SetSystemSignature<GeneralInputSystem>(signature);
    }

    auto  playerInputSystem = ecs.RegisterSystem<PlayerInputSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CPlayer>());
        ecs.SetSystemSignature<PlayerInputSystem>(signature);
    }

    renderSystem->Init();
    lightSystem->Init();
    generalInputSystem->Init();

    Entity playerEntity = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        playerEntity,
        CTransform{
            .position = glm::vec3(0.0f, 1.8f, 0.0f),
            .scale = glm::vec3(0.0f, 0.0f, 0.0f),
            .rotation = glm::vec3(0.0f, 0.0f, 0.0f),
        });

    ecs.AddComponent<CModel>(
        playerEntity,
        CModel{
            .model = AssetManager::GetModel("debugCube")
        });

    /*ecs.AddComponent<CLight>(
        playerEntity,
        CLight{
            .type = SPOT,
            .ambient = glm::vec3(0.0f),
            .diffuse = glm::vec3(0.2f, 0.2f, 0.2f),
            .specular = glm::vec3(1.0f, 1.0f, 1.0f),
            .radius = 9.0f,
            .direction = glm::vec3(0.0f, 0.0f, -1.0f),
            .innerCutoff = 12.5f,
            .outerCutoff = 17.5f,
        });*/

    ecs.AddComponent<CPlayer>(
        playerEntity,
        CPlayer{
            .flashlightOn = true,
            .movementSpeed = 3.0f,
        });

   Entity sponzaEntity = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        sponzaEntity,
        CTransform{
            .position = glm::vec3(0.0f, 0.0f, 0.0f),
            .scale = glm::vec3(1.0f, 1.0f, 1.0f),
            .rotation = glm::vec3(0.0f, 90.0f, 0.0f),
        });

    ecs.AddComponent<CModel>(
        sponzaEntity,
        CModel{
            .model = AssetManager::GetModel("sponza")
        });

    Entity lampEntity1 = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        lampEntity1,
        CTransform{
            .position = glm::vec3(-0.330f, 3.600f, -6.610f),
            .scale = glm::vec3(0.359f),
            .rotation = glm::vec3(0.0f, 180.0f, 0.0f),
        });

    ecs.AddComponent<CModel>(
        lampEntity1,
        CModel{
            .model = AssetManager::GetModel("victorianLamp")
        });

    ecs.AddComponent<CLight>(
        lampEntity1,
        CLight{
            .type = POINT,
            .diffuse = glm::vec3(0.725f, 0.529f, 0.286f),
            .radius = 7.57993f,
            .shadowCaster = true,
            .offset = glm::vec3(0.000f, -0.282f, 0.562f)
        });

    Entity lampEntity2 = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        lampEntity2,
        CTransform{
            .position = glm::vec3(-0.330f, 3.600f, 7.645f),
            .scale = glm::vec3(0.359f),
            .rotation = glm::vec3(0.0f, 0.0f, 0.0f),
        });

    ecs.AddComponent<CModel>(
        lampEntity2,
        CModel{
            .model = AssetManager::GetModel("victorianLamp")
        });

    ecs.AddComponent<CLight>(
        lampEntity2,
        CLight{
            .type = POINT,
            .diffuse = glm::vec3(0.725f, 0.529f, 0.286f),
            .radius = 7.75744f,
            .shadowCaster = true,
            .offset = glm::vec3(0.000f, -0.282f, -0.562f)
        });

    UI gui;

    gui.Init(window.GetWindow());

    lightSystem->Init();

    while (!glfwWindowShouldClose(window.GetWindow()))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        gui.NewFrame();

        generalInputSystem->Update(deltaTime, window.GetWindow());

        renderSystem->Update(deltaTime);

        playerInputSystem->Update(deltaTime);

        lightSystem->Update(deltaTime);

        gui.Update();
        window.Update();
    }

    gui.Terminate();
    
    window.Terminate();
    return 0;
}


