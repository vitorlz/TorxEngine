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
#include "Components/CRigidBody.h"

#include "Systems/RenderSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/GeneralInputSystem.h"
#include "Systems/playerInputSystem.h"
#include "Systems/PhysicsSystem.h"

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
    ecs.RegisterComponent<CRigidBody>();

    auto renderSystem = ecs.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        signature.set(ecs.GetComponentType<CModel>());
        ecs.SetSystemSignature<RenderSystem>(signature);
    }

    auto physicsSystem = ecs.RegisterSystem<PhysicsSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        signature.set(ecs.GetComponentType<CRigidBody>());
        ecs.SetSystemSignature<PhysicsSystem>(signature);
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
            .color = glm::vec3(1.0f),
            .radius = 9.0f,
            .strength = 1.0f,
            .direction = glm::vec3(0.0f, 0.0f, -1.0f),
            .innerCutoff = 12.5f,
            .outerCutoff = 17.5f,
        });

    ecs.AddComponent<CPlayer>(
        playerEntity,
        CPlayer{
            .flashlightOn = true,
            .movementSpeed = 3.0f,
        });


    for (int k = 0; k < 3; k++)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                Entity testCube = ecs.CreateEntity();

                ecs.AddComponent<CTransform>(
                    testCube,
                    CTransform{
                        .position = glm::vec3(0.5f * i, 2.0f + 0.5f * k, 0.5f * j),
                        .scale = glm::vec3(0.5f),
                        .rotation = glm::vec3(0.0f, 0.0f, 0.0f),
                    });

                ecs.AddComponent<CModel>(
                    testCube,
                    CModel{
                        .model = AssetManager::GetModel("dirtBlock")
                    });

                ecs.AddComponent<CRigidBody>(
                    testCube,
                    CRigidBody{
                        .mass = 1.f
                    });
            }
        }
    }
       
    Entity sponzaEntity = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        sponzaEntity,
        CTransform{
            .position = glm::vec3(0.0f, 0.0f, 0.0f),
            .scale = glm::vec3(1.0f, 1.0f, 1.0f),
            .rotation = glm::vec3(0.0f, 90.0f, 0.0f),
            .rotationMatrix = glm::mat4_cast(glm::quat(glm::vec3(0.0f, 90.0f, 0.0f))),
        });

    ecs.AddComponent<CModel>(
        sponzaEntity,
        CModel{
            .model = AssetManager::GetModel("sponza")
        });

    ecs.AddComponent<CRigidBody>(
        sponzaEntity,
        CRigidBody{
            .mass = 0.f
        });

   /*Entity lampEntity1 = ecs.CreateEntity();

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
            .color = glm::vec3(0.725f, 0.529f, 0.286f),
            .radius = 7.57993f,
            .strength = 1.0f,
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
            .color = glm::vec3(0.725f, 0.529f, 0.286f),
            .radius = 7.75744f,
            .strength = 1.0f,
            .shadowCaster = true,
            .offset = glm::vec3(0.000f, -0.282f, -0.562f)
        });*/

    UI gui;

    gui.Init(window.GetWindow());

    lightSystem->Init();
    physicsSystem->Init();

    while (!glfwWindowShouldClose(window.GetWindow()))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        gui.NewFrame();

        generalInputSystem->Update(deltaTime, window.GetWindow());

        renderSystem->Update(deltaTime);

        physicsSystem->Update(deltaTime);

        playerInputSystem->Update(deltaTime);

        lightSystem->Update(deltaTime);

        gui.Update();
        window.Update();
    }

    gui.Terminate();
    
    window.Terminate();
    return 0;
}


