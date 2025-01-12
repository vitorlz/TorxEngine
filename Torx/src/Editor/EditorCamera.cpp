#include "EditorCamera.h"
#include "../Components/CSingleton_Input.h"
#include "../UI/UI.h"
#include <algorithm>
#include "../include/Engine.h"
#include "../Util/Window.h"

static CSingleton_Input& inputSing = CSingleton_Input::getInstance();


EditorCamera::EditorCamera()
    : m_viewMatrix(glm::mat4(1.0f)),
    m_projection(glm::perspective(glm::radians(45.0f), (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT, 0.1f, 100.0f)),
    m_fov(45.0f)
{ 
}

glm::mat4& EditorCamera::GetProjMatrix()
{
    return m_projection;
}

glm::mat4& EditorCamera::GetViewMatrix()
{
    return m_viewMatrix;
}

glm::vec3 EditorCamera::GetCamPos()
{
    return m_transform.position;
}

glm::vec3 EditorCamera::GetFront()
{
    return m_front;
}

glm::vec3 EditorCamera::GetRight()
{
    return m_right;
}

glm::vec3 EditorCamera::GetUp()
{
    return m_up;
}

void EditorCamera::SetFov(float fov)
{
    m_fov = fov;
}

EditorCamera& EditorCamera::getInstance()
{
    static EditorCamera instance;

    return instance;
}

void EditorCamera::Update(float dt)
{
    if (Torx::Engine::MODE == Torx::PLAY) 
    {
        return;
    }   

    static glm::vec3 rotationEuler;

    
    if (inputSing.pressedKeys[MOUSE_MIDDLE] && !inputSing.pressedKeys[MOUSE_MIDDLE_SHIFT])
    {
        rotationEuler.x += inputSing.mouseOffsetY * 0.1f;
        rotationEuler.y += inputSing.mouseOffsetX * 0.1f;
        rotationEuler.x = std::clamp(rotationEuler.x, -90.0f, 90.0f);
    }
  
    m_transform.rotation = glm::quat(glm::radians(rotationEuler));

    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_transform.position);
    glm::mat4 rotMatrix = glm::mat4_cast(m_transform.rotation);
    model *= rotMatrix;

    glm::vec3 cameraRight = glm::normalize(glm::vec3(model[0]));
    glm::vec3 cameraUp = glm::normalize(glm::vec3(model[1]));
    glm::vec3 cameraFront = -glm::normalize(glm::vec3(model[2]));

    m_right = cameraRight;
    m_up = cameraUp;
    m_front = cameraFront;

    if (inputSing.pressedKeys[MOUSE_MIDDLE_SHIFT])
    {

        float sensitivity = 5.0f;

        m_transform.position += cameraUp * sensitivity * ((float)inputSing.mouseOffsetY / (float)Common::SCR_HEIGHT);
        m_transform.position -= cameraRight * sensitivity * ((float)inputSing.mouseOffsetX / (float)Common::SCR_WIDTH);
    }

    model = glm::mat4(1.0f);
    model = glm::translate(model, m_transform.position);
    rotMatrix = glm::mat4_cast(m_transform.rotation);
    model *= rotMatrix;

    m_viewMatrix = glm::inverse(model);

    Common::currentViewMatrix = m_viewMatrix;
    Common::currentProjMatrix = glm::perspective(glm::radians(m_fov), (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT, 0.1f, 100.0f);
    Common::currentCamPos = m_transform.position;
}