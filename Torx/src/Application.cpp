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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 800;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX{ 400 };
float lastY{ 300 };
bool firstMouse = true;
bool menu = false;
bool firstMouseUpdateAfterMenu = true;

Coordinator ecs;

int main()
{
    ecs.Init();

    // glfw: initialize and configure
    // ------------------------------

    Window window(SCR_WIDTH, SCR_HEIGHT, "Torx");

    window.DisableVsync();

    glfwSetCursorPosCallback(window.GetWindow(), mouse_callback);
    glfwSetKeyCallback(window.GetWindow(), key_callback);

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
    if (!menu)
    {
        if (firstMouse || firstMouseUpdateAfterMenu) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
            firstMouseUpdateAfterMenu = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset, true);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
    if (InputManager::GetKey(TAB)) {
        menu = !menu;
        firstMouseUpdateAfterMenu = true;
        if (!menu)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
