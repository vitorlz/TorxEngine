#include "PlayerInputSystem.h"

#include "../Components/CSingleton_Input.h" 
#include "../Core/Coordinator.hpp"

void PlayerInputSystem::Init() {}

extern Coordinator ecs;

void PlayerInputSystem::Update(float deltaTime, Camera& camera)
{

    // maybe make this change a playerCommand component that has like playerForward, playerPrimaryFire.
    // ultimately we want to have an entity with a transform and a player component (just a bool) and update the transform of that player entity to make the player move.
    // To do that we need to be able to calculate the view matrix in the rendering system using the player's transform. 
    CSingleton_Input& inputSing = CSingleton_Input::getInstance();
	// do player movement system here. For now you can just get the process keyboard function of the camera class and kind of put it here.
	// later we will want to create a camera for the player using his transform.
    float velocity = camera.MovementSpeed * deltaTime;

    if (inputSing.pressedKeys[W])
        camera.Position += camera.Front * velocity;
    if (inputSing.pressedKeys[S])
        camera.Position -= camera.Front * velocity;
    if (inputSing.pressedKeys[A])
        camera.Position -= camera.Right * velocity;
    if (inputSing.pressedKeys[D])
        camera.Position += camera.Right * velocity;
    if (inputSing.pressedKeys[SPACE])
        camera.Position += camera.WorldUp * velocity;
    if (inputSing.pressedKeys[LEFT_CONTROL])
        camera.Position -= camera.WorldUp * velocity;
    if (inputSing.pressedKeys[SHIFT_W])
        camera.Position += camera.Front * velocity * 2.0f;
    if (inputSing.pressedKeys[SHIFT_S])
        camera.Position -= camera.Front * velocity * 2.0f;
    if (inputSing.pressedKeys[SHIFT_A])
        camera.Position -= camera.Right * velocity * 2.0f;
    if (inputSing.pressedKeys[SHIFT_D])
        camera.Position += camera.Right * velocity * 2.0f;
}