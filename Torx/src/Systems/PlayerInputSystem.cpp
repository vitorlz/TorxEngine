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

void PlayerInputSystem::Init() {}
extern Coordinator ecs;
CSingleton_Input& inputSing = CSingleton_Input::getInstance();
void flashlightLogic(CPlayer& player, CLight& light);

void PlayerInputSystem::Update(float deltaTime)
{
    // maybe make this change settings in the player component that has like playerForward, playerPrimaryFire.

    static glm::vec3 rotationEuler;

    for (auto& entity : mEntities)
    {
        auto& transform = ecs.GetComponent<CTransform>(entity);
        auto& player = ecs.GetComponent<CPlayer>(entity);

        float velocity = player.movementSpeed * deltaTime;

        if (!UI::isOpen) {
            rotationEuler.x -= inputSing.mouseOffsetY * 0.05; 
            rotationEuler.y -= inputSing.mouseOffsetX * 0.05;
            rotationEuler.x = std::clamp(rotationEuler.x, -90.0f, 90.0f);
        }

        transform.rotation = glm::quat(glm::radians(rotationEuler));

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