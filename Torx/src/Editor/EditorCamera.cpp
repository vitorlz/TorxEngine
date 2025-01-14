#include "EditorCamera.h"
#include "../Components/CSingleton_Input.h"
#include "../UI/UI.h"
#include <algorithm>
#include "../include/Engine.h"
#include "../Util/Window.h"
#include "../Util/Util.h"


//#include <glm/gtc/quaternion.hpp>

static CSingleton_Input& inputSing = CSingleton_Input::getInstance();


EditorCamera::EditorCamera()
    : m_viewMatrix(glm::mat4(1.0f)),
    m_projection(glm::perspective(glm::radians(45.0f), (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT, 0.1f, 100.0f)),
    m_zOffset(0.0f)
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

EditorCameraTransform EditorCamera::GetTransform()
{
    return m_transform;
}

void EditorCamera::SetTransform(EditorCameraTransform transform)
{
    m_transform = transform;
}


glm::vec3 EditorCamera::GetCamPos()
{
    return m_transform.position;
}

void EditorCamera::SetCamPos(glm::vec3 pos)
{
    m_transform.position = pos;
}

glm::vec3 EditorCamera::GetFront()
{
    return m_front;
}

void EditorCamera::SetFront(glm::vec3 front)
{
    m_front = front;
}

glm::vec3 EditorCamera::GetRight()
{
    return m_right;
}

void EditorCamera::SetRight(glm::vec3 right)
{
    m_right = right;
}

glm::vec3 EditorCamera::GetUp()
{
    return m_up;
}

void EditorCamera::SetUp(glm::vec3 up)
{
    m_up = up;
}

void EditorCamera::SetZOffset(float zOffset)
{
    if (!UI::hovering)
    {
        m_zOffset = zOffset;
    }
}

EditorCamera& EditorCamera::getInstance()
{
    static EditorCamera instance;

    return instance;
}

void EditorCamera::Update(float dt)
{
    if (Torx::Engine::MODE != Torx::EDITOR) 
    {
        return;
    }   

    // middle mouse button to pan the camera
    if (inputSing.pressedKeys[MOUSE_MIDDLE] && !inputSing.pressedKeys[MOUSE_MIDDLE_SHIFT])
    {
        float pitchDelta = glm::radians(inputSing.mouseOffsetY * 0.1f);
        float yawDelta = glm::radians(inputSing.mouseOffsetX * 0.1f);

        glm::quat pitchQuat = glm::angleAxis(pitchDelta, glm::vec3(1.0f, 0.0f, 0.0f)); 
        glm::quat yawQuat = glm::angleAxis(yawDelta, glm::vec3(0.0f, 1.0f, 0.0f));     
       
        m_transform.rotation = yawQuat * m_transform.rotation * pitchQuat;
    }

    glm::mat4 model = glm::translate(glm::mat4(1.0f), m_transform.position);
    glm::mat4 rotMatrix = glm::mat4_cast(m_transform.rotation);
    model *= rotMatrix;

    glm::vec3 cameraRight = glm::normalize(glm::vec3(model[0]));
    glm::vec3 cameraUp = glm::normalize(glm::vec3(model[1]));
    glm::vec3 cameraFront = -glm::normalize(glm::vec3(model[2]));

    m_right = cameraRight;
    m_up = cameraUp;
    m_front = cameraFront;

    // middle mouse button + shift to move in the x and y axis
    if (inputSing.pressedKeys[MOUSE_MIDDLE_SHIFT])
    {
        float sensitivity = 7.0f;

        m_transform.position += cameraUp * sensitivity * ((float)inputSing.mouseOffsetY / (float)Common::SCR_HEIGHT);
        m_transform.position -= cameraRight * sensitivity * ((float)inputSing.mouseOffsetX / (float)Common::SCR_WIDTH);
    }

    // Scroll to move in z axis

    if (!UI::hovering)
    {
        float speed = 0.5f;
        m_transform.position += cameraFront * speed * m_zOffset;
        m_zOffset = 0;
    }
    

    model = glm::mat4(1.0f);
    model = glm::translate(model, m_transform.position);
    rotMatrix = glm::mat4_cast(m_transform.rotation);
    model *= rotMatrix;

    m_viewMatrix = glm::inverse(model);

    Common::currentViewMatrix = m_viewMatrix;
    Common::currentProjMatrix = glm::perspective(glm::radians(45.0f), (float)Common::SCR_WIDTH / (float)Common::SCR_HEIGHT, 0.1f, 100.0f);
    Common::currentCamPos = m_transform.position;
}