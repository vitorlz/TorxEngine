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
#include "../Engine.h"
#include "../Util/Util.h"
#include "../Components/CNativeScript.h"

extern Coordinator ecs;

class TestController : public ScriptableEntity
{
    CSingleton_Input& inputSing = CSingleton_Input::getInstance();

    void onCreate()
    {
        std::cout << "script created" << "\n";
    }

    void onUpdate(float dt)
    {
        if (!ecs.HasComponent<CTransform>(m_entity))
        {
            return;
        }

        auto& transform = ecs.GetComponent<CTransform>(m_entity);
      
        float velocity = 3.0f * dt;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.position);
        glm::mat4 rotMatrix = glm::mat4_cast(transform.rotation);
        model *= rotMatrix;

        glm::vec3 right = glm::normalize(glm::vec3(model[0]));
        glm::vec3 up = glm::normalize(glm::vec3(model[1]));
        glm::vec3 front = -glm::normalize(glm::vec3(model[2]));

        if (inputSing.keyDown[TORX_KEY_UP])
            transform.position += front * velocity;
        if (inputSing.keyDown[TORX_KEY_DOWN])
            transform.position -= front * velocity;
        if (inputSing.keyDown[TORX_KEY_RIGHT])
            transform.position += right * velocity;
        if (inputSing.keyDown[TORX_KEY_LEFT])
            transform.position -= right * velocity;

        if (ecs.HasComponent<CLight>(m_entity))
        {
            auto& light = ecs.GetComponent<CLight>(m_entity);

            light.isDirty = true;  
        }
    }
};


