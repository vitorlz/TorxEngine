#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "UI/UI.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Util/Shader.h"
#include "Util/Window.h"
#include "Core/InputManager.h"
#include "Util/Camera.h"
#include "Core/Coordinator.hpp"
#include "Util/ShaderManager.h"
#include "AssetLoading/AssetManager.h"


#include "Components/CTransform.h"
#include "Components/CMesh.h"

#include "Systems/RenderSystem.h"

#include <iostream>

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
Coordinator ecs;

int main()
{
    ecs.Init();

    // glfw: initialize and configure
    // ------------------------------

    Window window(SCR_WIDTH, SCR_HEIGHT, "Torx");

    window.DisableVsync();

    glfwSetCursorPosCallback(window.GetWindow(), mouse_callback);

    float deltaTime{};
    float lastFrame{};
    // render loop
    // -----------

    // should prob write a model manager

    AssetManager::LoadModels();

    ecs.RegisterComponent<CTransform>();
    ecs.RegisterComponent<CMesh>();

    auto renderSystem = ecs.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
        signature.set(ecs.GetComponentType<CMesh>());
    }

    renderSystem->Init();

    Entity backpackEntity = ecs.CreateEntity();
    
    ecs.AddComponent<CTransform>(
        backpackEntity, 
        CTransform{
            .position = glm::vec3(0.0f, 0.0f, 0.0f),
            .rotation = glm::vec3(0.0f, 0.0f, 0.0f),
            .scale = glm::vec3(1.0f, 1.0f, 1.0f)
        });

    ecs.AddComponent<CMesh>(
        backpackEntity,
        CMesh{
            .meshes = AssetManager::GetModel("backpack").meshes
        });

    UI gui;

    gui.Init(window.GetWindow());

    ShaderManager::LoadShaders();

    while (!glfwWindowShouldClose(window.GetWindow()))
    {

        // calculate deltatime
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        window.ProcessInputs();
        camera.ProcessKeyboard(deltaTime);

        gui.NewFrame();
 
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

