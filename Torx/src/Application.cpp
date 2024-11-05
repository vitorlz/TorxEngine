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

    ecs.AddComponent<CLight>(
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
        });

    ecs.AddComponent<CPlayer>(
        playerEntity,
        CPlayer{
            .flashlightOn = true
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

    Entity pointLight = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        pointLight,
        CTransform{
            .position = glm::vec3(0.0f, 4.0f, -2.0f),
            .scale = glm::vec3(0.2f, 0.2f, 0.2f),
            .rotation = glm::vec3(0.0f),
        });

    ecs.AddComponent<CLight>(
        pointLight,
        CLight{
            .type = POINT,
            .ambient = glm::vec3(0.0f),
            .diffuse = glm::vec3(0.5f, 0.5f, 0.5f),
            .specular = glm::vec3(1.0f, 1.0f, 1.0f),
            .radius = 9.0f,
            .shadowCaster = true,
        });

    ecs.AddComponent<CModel>(pointLight,
        CModel{
            .model = AssetManager::GetModel("debugCube")
        });

    Entity pointLight2 = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        pointLight2,
        CTransform{
            .position = glm::vec3(0.0f, 4.0f, 0.0f),
            .scale = glm::vec3(0.2f, 0.2f, 0.2f),
            .rotation = glm::vec3(0.0f),
        });

    ecs.AddComponent<CLight>(
        pointLight2,
        CLight{
            .type = POINT,
            .ambient = glm::vec3(0.0f),
            .diffuse = glm::vec3(0.5f, 0.5f, 0.5f),
            .specular = glm::vec3(1.0f, 1.0f, 1.0f),
            .radius = 9.0f,
            .shadowCaster = true,
        });

    ecs.AddComponent<CModel>(pointLight2,
        CModel{
            .model = AssetManager::GetModel("debugCube")

        });

    Entity pointLight3 = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        pointLight3,
        CTransform{
            .position = glm::vec3(0.0f, 4.0f, 2.0f),
            .scale = glm::vec3(0.2f, 0.2f, 0.2f),
            .rotation = glm::vec3(0.0f),
        });

    ecs.AddComponent<CLight>(
        pointLight3,
        CLight{
            .type = POINT,
            .ambient = glm::vec3(0.0f),
            .diffuse = glm::vec3(0.5f, 0.5f, 0.5f),
            .specular = glm::vec3(1.0f, 1.0f, 1.0f),
            .radius = 9.0f,
            .shadowCaster = true,
        });

    ecs.AddComponent<CModel>(pointLight3,
        CModel{
            .model = AssetManager::GetModel("debugCube")

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


