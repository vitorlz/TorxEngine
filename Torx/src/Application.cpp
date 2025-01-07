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

#include "Scene/Scene.h"

#include "Components/CTransform.h"
#include "Components/CLight.h"
#include "Components/CModel.h"
#include "Components/CSingleton_Input.h"
#include "Components/CPlayer.h"
#include "Components/CRigidBody.h"
#include "Components/CMesh.h"
#include "Components/CAnimator.h"

#include "Systems/RenderSystem.h"
#include "Rendering/RenderingUtil.h"
#include "Systems/LightSystem.h"
#include "Systems/GeneralInputSystem.h"
#include "Systems/playerInputSystem.h"
#include "Systems/PhysicsSystem.h"
#include "Systems/AnimationSystem.h"
#include <filesystem>
#include "Rendering/TextRendering.h"

#include <iostream>

#include <ft2build.h>

// settings

Coordinator ecs;

int main()
{
    ecs.Init();

    Window window(Common::SCR_WIDTH, Common::SCR_HEIGHT, "Torx");

    float deltaTime{};
    float lastFrame{};

    AssetManager::LoadModels();
    AssetManager::LoadMeshes();
    AssetManager::LoadAnimations();
    ShaderManager::LoadShaders();

    ecs.RegisterComponent<CTransform>();
    ecs.RegisterComponent<CModel>();
    ecs.RegisterComponent<CLight>();
    ecs.RegisterComponent<CPlayer>();
    ecs.RegisterComponent<CRigidBody>();
    ecs.RegisterComponent<CMesh>();
    ecs.RegisterComponent<CAnimator>();

    auto renderSystem = ecs.RegisterSystem<RenderSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CTransform>());
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

    auto  animationSystem = ecs.RegisterSystem<AnimationSystem>();
    {
        Signature signature;
        signature.set(ecs.GetComponentType<CAnimator>());
        ecs.SetSystemSignature<AnimationSystem>(signature);
    }

    UI gui;

    gui.Init(window.GetWindow());

    RenderingUtil::Init();

    TextRendering::Init();
    AssetManager::LoadFonts();

    Scene::LoadSceneFromJson("testscene3.json");

    renderSystem->Init();
    generalInputSystem->Init();
    lightSystem->Init();
    physicsSystem->Init();

     
    while (!glfwWindowShouldClose(window.GetWindow()))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        gui.NewFrame();

        generalInputSystem->Update(deltaTime, window.GetWindow());

        animationSystem->Update(deltaTime);

        renderSystem->Update(deltaTime);

        physicsSystem->Update(deltaTime);

        playerInputSystem->Update(deltaTime);

        lightSystem->Update(deltaTime);

        gui.Update();

        window.Update();
    }

    gui.Terminate();
    FT_Done_FreeType(TextRendering::m_ft);
    window.Terminate();
    return 0;
}


