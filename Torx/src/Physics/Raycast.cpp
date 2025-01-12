#include "Raycast.h"
#include "../Core/Common.h"
#include "../UI/UI.h"
#include <iostream>
#include "../include/Engine.h"

glm::vec4 Raycast::m_MouseRayStartWorld{};
glm::vec3 Raycast::m_MouseRayDirWorld{};
glm::vec4 Raycast::m_MouseRayEndWorld{};
btVector3 Raycast::m_MouseHitPointWorld{};
CSingleton_Input& Raycast::m_inputSing{ CSingleton_Input::getInstance() };
btDiscreteDynamicsWorld* Raycast::m_dynamicsWorld{};
int Raycast::m_SelectedEntity;

void Raycast::calculateMouseRaycast(glm::mat4 projView)
{
	glm::vec4 rayStartNDC(
		(m_inputSing.mouseX / (double)Common::SCR_WIDTH - 0.5f) * 2.0f,
		(((double)Common::SCR_HEIGHT - m_inputSing.mouseY) / (double)Common::SCR_HEIGHT - 0.5f) * 2.0f,
		-1.0f,
		1.0f
		);


	glm::vec4 rayEndNDC(
		(m_inputSing.mouseX / (double)Common::SCR_WIDTH - 0.5f) * 2.0f,
		(((double)Common::SCR_HEIGHT - m_inputSing.mouseY) / (double)Common::SCR_HEIGHT - 0.5f) * 2.0f,
		0.0f,
		1.0f
	);

	glm::mat4 inverseProjView = glm::inverse(projView);

	glm::vec4 rayStartWorld = inverseProjView * rayStartNDC;
	glm::vec4 rayEndWorld = inverseProjView * rayEndNDC;

	m_MouseRayStartWorld = rayStartWorld / rayStartWorld.w;
	m_MouseRayEndWorld = rayEndWorld / rayEndWorld.w;

	m_MouseRayDirWorld = normalize(m_MouseRayEndWorld - m_MouseRayStartWorld);
}

unsigned int Raycast::mouseRaycast()
{
	glm::vec3 rayOrigin = m_MouseRayStartWorld;
	glm::vec3 rayEnd = rayOrigin + m_MouseRayDirWorld * 1000.0f;

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

int Raycast::getSelectedEntity()
{
	CSingleton_Input& inputSing = CSingleton_Input::getInstance();

	static bool rayFired{ false };
	if (Torx::Engine::MODE == Torx::EDITOR)
	{
		if (inputSing.pressedKeys[MOUSE_RIGHT] && !rayFired)
		{
			m_SelectedEntity = mouseRaycast();
			rayFired = true;
		}
		else if (!inputSing.pressedKeys[MOUSE_RIGHT])
		{
			rayFired = false;
		}
	}

	return m_SelectedEntity;
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