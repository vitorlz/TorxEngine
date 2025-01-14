#include "PlayerInputSystem.h"

#include "../Components/CSingleton_Input.h" 
#include "../Components/CTransform.h"
#include "../Components/CPlayer.h"
#include "../Components/CLight.h"
#include "../Core/Coordinator.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../UI/UI.h"
#include "../include/Engine.h"
#include "../Util/Util.h"

void PlayerInputSystem::Init() {}
extern Coordinator ecs;
static CSingleton_Input& inputSing = CSingleton_Input::getInstance();
void flashlightLogic(CPlayer& player, CLight& light);

void PlayerInputSystem::Update(float deltaTime)
{
    // maybe make this change settings in the player component that has like playerForward, playerPrimaryFire.


    if (Torx::Engine::MODE != Torx::PLAY)
    {
        return;
    }
   // static glm::vec3 rotationEuler;

    for (auto& entity : mEntities)
    {
        auto& transform = ecs.GetComponent<CTransform>(entity);
        auto& player = ecs.GetComponent<CPlayer>(entity);

        float velocity = player.movementSpeed * deltaTime;

        // change this later so that this whole function only updated if the engine is in play mode

        float pitchDelta = glm::radians(-inputSing.mouseOffsetY * 0.05f);
        float yawDelta = glm::radians(-inputSing.mouseOffsetX * 0.05f);

        glm::quat pitchQuat = glm::angleAxis(pitchDelta, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::quat yawQuat = glm::angleAxis(yawDelta, glm::vec3(0.0f, 1.0f, 0.0f));

        transform.rotation = yawQuat * transform.rotation * pitchQuat;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);
        glm::mat4 rotMatrix = glm::mat4_cast(transform.rotation);
        model *= rotMatrix;
         
        glm::vec3 cameraRight = glm::normalize(glm::vec3(model[0]));
        glm::vec3 cameraUp = glm::normalize(glm::vec3(model[1]));
        glm::vec3 cameraFront = -glm::normalize(glm::vec3(model[2]));

        if (inputSing.pressedKeys[W])
            transform.position += cameraFront * velocity;
        if (inputSing.pressedKeys[S])
            transform.position -= cameraFront * velocity;
        if (inputSing.pressedKeys[A])
            transform.position -= cameraRight * velocity;
        if (inputSing.pressedKeys[D])
            transform.position += cameraRight * velocity;
        if (inputSing.pressedKeys[SPACE])
            transform.position += glm::vec3(0, 1, 0) * velocity;
        if (inputSing.pressedKeys[LEFT_CONTROL])
            transform.position -= glm::vec3(0, 1, 0) * velocity;
        if (inputSing.pressedKeys[SHIFT_W])
            transform.position += cameraFront * velocity * 2.0f;
        if (inputSing.pressedKeys[SHIFT_S])
            transform.position -= cameraFront * velocity * 2.0f;
        if (inputSing.pressedKeys[SHIFT_A])
            transform.position -= cameraRight * velocity * 2.0f;
        if (inputSing.pressedKeys[SHIFT_D])
            transform.position += cameraRight * velocity * 2.0f;

        model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        rotMatrix = glm::mat4_cast(transform.rotation);
        model *= rotMatrix;

        glm::mat4 viewMatrix = glm::inverse(model);

        player.viewMatrix = viewMatrix;
        player.front = cameraFront;
        player.right = cameraRight;
        player.up = cameraUp;

        if (ecs.HasComponent<CLight>(entity))
        {
            auto& light = ecs.GetComponent<CLight>(entity);
            flashlightLogic(player, light);    
        }

        Common::currentViewMatrix = player.viewMatrix;
        Common::currentProjMatrix = player.projectionMatrix;
        Common::currentCamPos = transform.position;
    }
}

float previousRadius;
void flashlightLogic(CPlayer& player, CLight& light)
{
    static bool flashlightSwitch = player.flashlightOn ? false : true;

    if (light.type == SPOT)
    {
        light.direction = player.front;
    }
    if (player.flashlightOn)
    {
        light.isDirty = true;
    }
    if (inputSing.pressedKeys[F])
    {
        if (!flashlightSwitch)
        {
            if (!player.flashlightOn)
            {
                light.radius = previousRadius;
                player.flashlightOn = true;
            }
            else if (player.flashlightOn)
            {
                previousRadius = light.radius;
                light.radius = 0.0f;
                player.flashlightOn = false;
            }
            flashlightSwitch = true;
        }
    }
    if (!inputSing.pressedKeys[F])
    {
        flashlightSwitch = false;
    }
}