#include <Engine.h>

int main()
{
    Torx::Engine& engine = Torx::Engine::getInstance();
    engine.ConfigWindow(1600, 900, "Sandbox");   
    engine.Init();
    engine.Run();
}