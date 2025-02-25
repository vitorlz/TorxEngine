#include <Core/Common.h>
#include <Engine.h>
#include "Editor/Editor.h"
#include "Engine.h"

#include "Components/CNativeScript.h"
#include "Game/PlayerInputScript.hpp"
#include "Game/TestController.hpp"

int main()
{
    Torx::Engine& engine = Torx::Engine::getInstance();
    engine.ConfigWindow(1600, 900, "TorxEditor");
    engine.Init();

    ScriptFactory::Register("PlayerController", []() { return new PlayerController(); });
    ScriptFactory::Register("TestController", []() { return new TestController(); });

    Torx::Engine::MODE = Torx::EDITOR;
    Editor& editor = Editor::getInstance();
    editor.InitUI();
    editor.Run();
}