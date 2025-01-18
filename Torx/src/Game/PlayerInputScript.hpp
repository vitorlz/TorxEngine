#pragma once


#include "../Components/CSingleton_Input.h" 
#include "../Components/CTransform.h"
#include "../Components/CPlayer.h"
#include "../Components/CLight.h"
#include "../Components/CCamera.h"
#include "../Core/Coordinator.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../UI/UI.h"
#include "../include/Engine.h"
#include "../Util/Util.h"
#include "../Components/CNativeScript.h"

extern Coordinator ecs;
static CSingleton_Input& inputSing = CSingleton_Input::getInstance();

class PlayerController : public ScriptableEntity
{
    void onUpdate(float dt)
    {

        if (!ecs.HasComponent<CTransform>(m_entity) || !ecs.HasComponent<CPlayer>(m_entity) || !ecs.HasComponent<CCamera>(m_entity))
        {
            return;
        }

        auto& transform = ecs.GetComponent<CTransform>(m_entity);
        auto& player = ecs.GetComponent<CPlayer>(m_entity);
        auto& cameraComp = ecs.GetComponent<CCamera>(m_entity);

        float velocity = player.movementSpeed * dt;

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

        if (inputSing.keyDown[TORX_KEY_W])
            transform.position += cameraFront * velocity;
        if (inputSing.keyDown[TORX_KEY_S])
            transform.position -= cameraFront * velocity;
        if (inputSing.keyDown[TORX_KEY_A])
            transform.position -= cameraRight * velocity;
        if (inputSing.keyDown[TORX_KEY_D])
            transform.position += cameraRight * velocity;
        if (inputSing.keyDown[TORX_KEY_SPACE])
            transform.position += glm::vec3(0, 1, 0) * velocity;
        if (inputSing.keyDown[TORX_KEY_LEFT_CONTROL])
            transform.position -= glm::vec3(0, 1, 0) * velocity;
        if (inputSing.keyDown[TORX_KEY_LEFT_SHIFT] && inputSing.keyDown[TORX_KEY_W])
            transform.position += cameraFront * velocity * 2.0f;
        if (inputSing.keyDown[TORX_KEY_LEFT_SHIFT] && inputSing.keyDown[TORX_KEY_S])
            transform.position -= cameraFront * velocity * 2.0f;
        if (inputSing.keyDown[TORX_KEY_LEFT_SHIFT] && inputSing.keyDown[TORX_KEY_A])
            transform.position -= cameraRight * velocity * 2.0f;
        if (inputSing.keyDown[TORX_KEY_LEFT_SHIFT] && inputSing.keyDown[TORX_KEY_D])
            transform.position += cameraRight * velocity * 2.0f;

        model = glm::mat4(1.0f);
        model = glm::translate(model, transform.position);
        rotMatrix = glm::mat4_cast(transform.rotation);
        model *= rotMatrix;

        // maybe just do this before rendering? the view matrix is simply the inverse of the model matrix, we dont have to do it here.
        // then we could just manipulate the model matrix of the player and get the view matrix "for free" by getting its inverse.
        glm::mat4 viewMatrix = glm::inverse(model);

        player.viewMatrix = viewMatrix;
        player.front = cameraFront;
        player.right = cameraRight;
        player.up = cameraUp;

        // this should be handled elsewhere I think.
        Common::currentViewMatrix = player.viewMatrix;
        Common::currentProjMatrix = cameraComp.projection;
        Common::currentCamPos = transform.position;
    }
};


