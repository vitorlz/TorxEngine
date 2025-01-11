
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

#include <iostream>

#include <ft2build.h>

int main()
{
  
    float deltaTime{};
    float lastFrame{};

    Torx::Engine engine;

    engine.ConfigWindow(Common::SCR_WIDTH, Common::SCR_HEIGHT, "Torx");

    engine.Init("testscene3.json");

    UI gui;

    gui.Init(engine.GetWindow().GetPointer());
     
    while (!glfwWindowShouldClose(engine.GetWindow().GetPointer()))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        gui.NewFrame();

        engine.Run(deltaTime);

        gui.Update();

        engine.GetWindow().Update();
    }

    gui.Terminate();
    FT_Done_FreeType(TextRendering::m_ft);
    engine.GetWindow().Terminate();
    return 0;
}


