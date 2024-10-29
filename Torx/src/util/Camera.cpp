#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Camera.h"
#include "../UI/UI.h"
#include "../Core/InputManager.h"

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 3.0f;
const float SENSITIVITY = 0.05f;
const float ZOOM = 45.0f;

// constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM), FPS(false)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    Camera::updateCameraVectors();
}

//Constructor with scalar values
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    Camera::updateCameraVectors();
}

// returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

// processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Camera::ProcessKeyboard(float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;

    if (FPS == true) {
        if (InputManager::GetKey(W))
            Position += Front * velocity;
        if (InputManager::GetKey(S))
            Position -= Front * velocity;
        if (InputManager::GetKey(A))
            Position -= Right * velocity;
        if (InputManager::GetKey(D))
            Position += Right * velocity;
     
        if (InputManager::GetKey(SHIFT_W))
            Position += Front * velocity * 2.0f;
        if (InputManager::GetKey(SHIFT_S))
            Position -= Front * velocity * 2.0f;
        if (InputManager::GetKey(SHIFT_A))
            Position -= Right * velocity * 2.0f;
        if (InputManager::GetKey(SHIFT_D))
            Position += Right * velocity * 2.0f;

        Position.y = 1.0f;
    }
    else {
        if (InputManager::GetKey(W))
            Position += Front * velocity;
        if (InputManager::GetKey(S))
            Position -= Front * velocity;
        if (InputManager::GetKey(A))
            Position -= Right * velocity;
        if (InputManager::GetKey(D))
            Position += Right * velocity;
        if (InputManager::GetKey(SPACE))
            Position += WorldUp * velocity;
        if (InputManager::GetKey(LEFT_CONTROL))
            Position -= WorldUp* velocity;

        if (InputManager::GetKey(SHIFT_W))
            Position += Front * velocity * 2.0f;
        if (InputManager::GetKey(SHIFT_S))
            Position -= Front * velocity * 2.0f;
        if (InputManager::GetKey(SHIFT_A))
            Position -= Right * velocity * 2.0f;
        if (InputManager::GetKey(SHIFT_D))
            Position += Right * velocity * 2.0f;
    }

}

float lastX{ 400 };
float lastY{ 300 };
bool firstMouse = true;
bool firstMouseUpdateAfterMenu = true;


// processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Camera::ProcessMouseMovement(float xpos, float ypos, GLboolean constrainPitch)
{

    if (firstMouse || UI::firstMouseUpdateAfterMenu) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        UI::firstMouseUpdateAfterMenu = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    Camera::updateCameraVectors();
}

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

// calculates the front vector from the Camera's (updated) Euler Angles
void Camera::updateCameraVectors() {
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}

//  toggle camera mode
void Camera::toggleCameraMode() {
    FPS = !FPS;
}

