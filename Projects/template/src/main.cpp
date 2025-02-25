#include <Engine.h>

int main()
{
    Torx::Engine& engine = Torx::Engine::getInstance();
    engine.ConfigWindow(1600, 900, "NEW_PROJECT_TITLE");   
    engine.Init();
    engine.Run();
}