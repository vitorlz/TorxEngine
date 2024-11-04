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
#include "Util/Camera.h"
#include "Core/Coordinator.hpp"
#include "Util/ShaderManager.h"
#include "AssetLoading/AssetManager.h"

#include "Components/CTransform.h"
#include "Components/CLight.h"
#include "Components/CModel.h"
#include "Components/CSingleton_Input.h"

#include "Systems/RenderSystem.h"
#include "Systems/LightSystem.h"
#include "Systems/GeneralInputSystem.h"
#include "Systems/playerInputSystem.h"

#include <iostream>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
// settings

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Coordinator ecs;

int main()
{
    ecs.Init();

    Window window(Common::SCR_WIDTH, Common::SCR_HEIGHT, "Torx");

    window.DisableVsync();

    glfwSetCursorPosCallback(window.GetWindow(), mouse_callback);
    glfwSetScrollCallback(window.GetWindow(), scroll_callback);

    float deltaTime{};
    float lastFrame{};

    AssetManager::LoadModels();
    ShaderManager::LoadShaders();

    ecs.RegisterComponent<CTransform>();
    ecs.RegisterComponent<CModel>();
    ecs.RegisterComponent<CLight>();

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
        ecs.SetSystemSignature<PlayerInputSystem>(signature);
    }

    renderSystem->Init();
    
    lightSystem->Init();

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
            .dynamic = false
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
            .dynamic = false
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
            .dynamic = false
        });

    ecs.AddComponent<CModel>(pointLight3,
        CModel{
            .model = AssetManager::GetModel("debugCube")

        });

    Entity flashlight = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        flashlight,
        CTransform{
            .position = camera.Position,
            .scale = glm::vec3(0.2f, 0.2f, 0.2f),
            .rotation = glm::vec3(0.0f)
        });

    ecs.AddComponent<CLight>(
        flashlight,
        CLight{
            .type = SPOT,
            .ambient = glm::vec3(0.0f),
            .diffuse = glm::vec3(0.2f, 0.2f, 0.2f),
            .specular = glm::vec3(1.0f, 1.0f, 1.0f),
            .radius = 0.032f,
            .direction = camera.Front,
            .innerCutoff = 12.5f,
            .outerCutoff = 17.5f,
            .dynamic = true
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

        renderSystem->Update(deltaTime, camera);

        lightSystem->Update(deltaTime, camera);
      
        generalInputSystem->Update(deltaTime, window.GetWindow());

        playerInputSystem->Update(deltaTime, camera);

        gui.Update();
        window.Update();
    }

    gui.Terminate();
    
    window.Terminate();
    return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) 
{
    if (UI::isOpen)
    {
        return;
    }
    
    camera.ProcessMouseMovement(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

