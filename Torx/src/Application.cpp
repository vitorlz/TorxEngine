#define GLFW_INCLUDE_NONE
#include "UI/UI.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Common.h"
#include "Util/Shader.h"
#include "Util/Window.h"
#include "Core/Coordinator.hpp"
#include "Rendering/TextRendering.h"
#include "../include/Engine.h"
#include <filesystem>
#include "Editor/EditorCamera.h"
#include "Components/CNativeScript.h"
#include "Components/CTransform.h"
#include "Game/PlayerInputScript.hpp"
#include "Game/TestController.hpp"
#include "Editor/Editor.h"

#include <iostream>

#include <ft2build.h>


int main()
{
    ScriptFactory::Register("PlayerController", []() { return new PlayerController(); });
    ScriptFactory::Register("TestController", []() { return new TestController(); });

    float deltaTime{};
    float lastFrame{};

    Torx::Engine& engine = Torx::Engine::getInstance();

    engine.ConfigWindow(Common::SCR_WIDTH, Common::SCR_HEIGHT, "Torx");
    engine.Init();

    Editor& editor = Editor::getInstance();
    editor.InitUI();
     
    while (!glfwWindowShouldClose(engine.GetWindow().GetPointer()))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        engine.Run(deltaTime);
        editor.Update(deltaTime);

        engine.GetWindow().Update();
    }

    editor.GetUI().Terminate();
    FT_Done_FreeType(TextRendering::m_ft);
    engine.GetWindow().Terminate();
    return 0;
}


