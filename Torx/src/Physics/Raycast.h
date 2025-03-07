#pragma once 

#include <glm/glm.hpp>
#include "../Components/CSingleton_Input.h"
#include <btBulletDynamicsCommon.h>
#include "../Core/Types.hpp"

class Raycast
{
public:
	static void calculateMouseRaycast(glm::mat4 projView);
	static unsigned int mouseRaycast(glm::mat4 projView, glm::vec4 ndcStart, glm::vec4 ndcEnd, float worldDistance);
	static glm::vec3 getMouseRayDir();
	static glm::vec3 getMouseRayStart();
	static btVector3 getMouseHitPointWorld();
	static void setSelectedEntity(int entity);
	static void setDynamicsWorld(btDiscreteDynamicsWorld* dynamicsWorld);
private: 
	static glm::vec4 m_MouseRayStartWorld;
	static glm::vec3 m_MouseRayDirWorld;
	static glm::vec4 m_MouseRayEndWorld;
	static btVector3 m_MouseHitPointWorld;
	static btDiscreteDynamicsWorld* m_dynamicsWorld;
	static CSingleton_Input& m_inputSing;
	static int m_SelectedEntity;
};