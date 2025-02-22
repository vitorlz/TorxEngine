#include "Raycast.h"
#include "../Core/Common.h"
//#include "../UI/UI.h"
#include <iostream>
#include "../Engine.h"

glm::vec4 Raycast::m_MouseRayStartWorld{};
glm::vec3 Raycast::m_MouseRayDirWorld{};
glm::vec4 Raycast::m_MouseRayEndWorld{};
btVector3 Raycast::m_MouseHitPointWorld{};
CSingleton_Input& Raycast::m_inputSing{ CSingleton_Input::getInstance() };
btDiscreteDynamicsWorld* Raycast::m_dynamicsWorld{};
int Raycast::m_SelectedEntity;

unsigned int Raycast::mouseRaycast(glm::mat4 projView, glm::vec4 ndcStart, glm::vec4 ndcEnd, float worldDistance)
{
	glm::mat4 inverseProjView = glm::inverse(projView);

	glm::vec4 rayStartWorld = inverseProjView * ndcStart;
	glm::vec4 rayEndWorld = inverseProjView * ndcEnd;

	m_MouseRayStartWorld = rayStartWorld / rayStartWorld.w;
	m_MouseRayEndWorld = rayEndWorld / rayEndWorld.w;

	m_MouseRayDirWorld = normalize(m_MouseRayEndWorld - m_MouseRayStartWorld);
	
	glm::vec3 rayOrigin = m_MouseRayStartWorld;
	glm::vec3 rayEnd = rayOrigin + m_MouseRayDirWorld * worldDistance;

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
		m_MouseHitPointWorld = RayCallback.m_hitPointWorld;
		return (int)RayCallback.m_collisionObject->getUserIndex();
	}
	
	return -1;
}

void Raycast::setDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld)
{
	m_dynamicsWorld = dynamicsWorld;
}

void Raycast::setSelectedEntity(int entity)
{
	m_SelectedEntity == entity;
}

glm::vec3 Raycast::getMouseRayDir() 
{
	return m_MouseRayDirWorld;
}

glm::vec3 Raycast::getMouseRayStart()
{
	return m_MouseRayStartWorld;
}
btVector3 Raycast::getMouseHitPointWorld()
{
	return m_MouseHitPointWorld;
}