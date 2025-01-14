#pragma once 
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct EditorCameraTransform
{
	glm::vec3 position;
	glm::quat rotation;
};

class EditorCamera
{
public:
	EditorCamera(EditorCamera const&) = delete;
	void operator = (EditorCamera const&) = delete;
	static EditorCamera& getInstance();

	glm::mat4& GetViewMatrix();
	glm::mat4& GetProjMatrix();
	glm::vec3 GetCamPos();
	void SetCamPos(glm::vec3 pos);
	EditorCameraTransform GetTransform();
	void SetTransform(EditorCameraTransform transform);
	glm::vec3 GetFront();
	void SetFront(glm::vec3 front);
	glm::vec3 GetRight();
	void SetRight(glm::vec3 right);
	glm::vec3 GetUp();
	void SetUp(glm::vec3 up);

	void SetZOffset(float zOffset);

	void Update(float dt);
private:
	EditorCamera();
	EditorCameraTransform m_transform;
	float m_zOffset;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projection;
	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_up;
};