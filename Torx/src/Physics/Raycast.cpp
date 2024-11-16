#include "Raycast.h"
#include "../Core/Common.h"
#include <iostream>

glm::vec3 Raycast::m_MouseRayStartWorld{};
glm::vec3 Raycast::m_MouseRayDirWorld{};
CSingleton_Input& Raycast::m_inputSing{ CSingleton_Input::getInstance() };
btDiscreteDynamicsWorld* Raycast::m_dynamicsWorld{};

void Raycast::calculateMouseRaycast(glm::mat4 projView)
{
	glm::vec4 rayStartNDC(
		((float)m_inputSing.mouseX / (float)Common::SCR_WIDTH - 0.5f) * 2.0f,
		(((float)Common::SCR_HEIGHT - (float)m_inputSing.mouseY) / (float)Common::SCR_HEIGHT - 0.5f) * 2.0f,
		-1.0f,
		1.0f
		);

	glm::vec4 rayEndNDC(
		((float)m_inputSing.mouseX / (float)Common::SCR_WIDTH - 0.5f) * 2.0f,
		(((float)Common::SCR_HEIGHT - (float)m_inputSing.mouseY) / (float)Common::SCR_HEIGHT - 0.5f) * 2.0f,
		0.0f,
		1.0f
	);

	glm::mat4 inverseProjView = glm::inverse(projView);

	glm::vec4 rayStartWorld = inverseProjView * rayStartNDC;
	glm::vec4 rayEndWorld = inverseProjView * rayEndNDC;

	m_MouseRayStartWorld = glm::vec3(rayStartWorld) / rayStartWorld.w;

	m_MouseRayDirWorld = normalize(glm::vec3((rayEndWorld / rayEndWorld.w)) - m_MouseRayStartWorld);
}

unsigned int Raycast::mouseRaycast()
{
	glm::vec3 rayOrigin = m_MouseRayStartWorld;
	glm::vec3 rayEnd = m_MouseRayStartWorld + m_MouseRayDirWorld * 1000.0f;

	btCollisionWorld::ClosestRayResultCallback RayCallback(
		btVector3(rayOrigin.x, rayOrigin.y, rayOrigin.z),
		btVector3(rayEnd.x, rayEnd.y, rayEnd.z)
	);

	m_dynamicsWorld->rayTest(
		btVector3(rayOrigin.x, rayOrigin.y, rayOrigin.z),
		btVector3(rayEnd.x, rayEnd.y, rayEnd.z),
		RayCallback
	);

	if (RayCallback.hasHit())
	{
		//std::cout << "mesh " << (int)RayCallback.m_collisionObject->getUserIndex() << "\n";
		return (int)RayCallback.m_collisionObject->getUserIndex();
	}
}

void Raycast::setDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld)
{
	m_dynamicsWorld = dynamicsWorld;
}

glm::vec3 Raycast::getMouseRayDir() 
{
	return m_MouseRayDirWorld;
}

glm::vec3 Raycast::getMouseRayStart()
{
	return m_MouseRayStartWorld;
}