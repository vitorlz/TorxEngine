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
#include "Core/InputManager.h"
#include "Util/Camera.h"
#include "Core/Coordinator.hpp"
#include "Util/ShaderManager.h"
#include "AssetLoading/AssetManager.h"

#include "Components/CTransform.h"
#include "Components/CMesh.h"
#include "Components/CLight.h"

#include "Systems/RenderSystem.h"
#include "Systems/LightSystem.h"

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
    ecs.RegisterComponent<CMesh>();
    ecs.RegisterComponent<CLight>();

    auto renderSystem = ecs.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        signature.set(ecs.GetComponentType<CMesh>());
        ecs.SetSystemSignature<RenderSystem>(signature);
    }

    auto lightSystem = ecs.RegisterSystem<LightSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        signature.set(ecs.GetComponentType<CLight>());
        ecs.SetSystemSignature<LightSystem>(signature);
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

    ecs.AddComponent<CMesh>(
        sponzaEntity,
        CMesh{
            .meshes = AssetManager::GetModel("sponza").meshes
        });

    Entity pointLight = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        pointLight,
        CTransform{
            .position = glm::vec3(0.0f, 4.0f, 0.0f),
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
            .quadratic = 0.3f
        });

    ecs.AddComponent<CMesh>(
        pointLight,
        CMesh{
            .meshes = AssetManager::GetModel("debugCube").meshes
        });

    Entity pointLight2 = ecs.CreateEntity();

    ecs.AddComponent<CTransform>(
        pointLight2,
        CTransform{
            .position = glm::vec3(1.0f, 4.0f, 0.0f),
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
            .quadratic = 0.3f
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
            .quadratic = 0.032f,
            .direction = camera.Front,
            .innerCutoff = 12.5f,
            .outerCutoff = 17.5f
        });

    UI gui;

    gui.Init(window.GetWindow());

    lightSystem->Init();

    while (!glfwWindowShouldClose(window.GetWindow()))
    {

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        window.ProcessInputs();
        camera.ProcessKeyboard(deltaTime);

        gui.NewFrame();

        lightSystem->Update(deltaTime, camera);
 
        renderSystem->Update(deltaTime, camera);

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

