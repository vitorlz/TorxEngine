   #include "LightSystem.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CLight.h"
#include "../Components/CPlayer.h"
#include "glad/glad.h"
#include "iostream"
#include "../Core/Common.h"
#include <unordered_map>

extern Coordinator ecs;

struct Light
{
	glm::vec4 position;
	glm::vec4 color;
	glm::vec4 direction;
	glm::vec4 offset;

	float type;
	float radius;
	float innerCutoff;
	float outerCutoff;
	
	bool shadowCaster;
	bool isDirty;
	bool padding1[2];
	float padding2[3];
};

std::vector<Light> lights;
std::unordered_map<Entity, Light> EntityToLightMap;
std::unordered_map<Entity, int> EntityToLightIndexMap;

Light lightData;

void LightSystem::Init() 
{
	mMaxLights = 30;
	mLightIndex = 0;

	// create ssbo for storing lights
	glCreateBuffers(1, &mSsbo);
	glNamedBufferStorage(mSsbo, sizeof(Light) * mMaxLights, (const void*) nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mSsbo);

	// First initialize the light system by putting the initial light data in the ssbo. We also keep a map from entities to lights and from entity to light index
	// so that we can update the light data of each entity in the update function. We do this to prevent copying data into light instances every single frame
	// for every entity. We create the light data instances during initialization, copy data into them, and fetch the light data of each respective entity in the update function. 
	// Turns out that copying data into instances like that is extremely expensive and was eating almost 30% of my FPS.

	for (const auto& entity : mEntities)
	{
		const auto& transform = ecs.GetComponent<CTransform>(entity);
		const auto& light = ecs.GetComponent<CLight>(entity);

		if (mEntities.size() > mLightIndex)
		{
			mLightIndex++;

			std::cout << "light index initialization" << mLightIndex << "\n";
		}

		std::cout << "light type: " << light.type << "\n";

		if (light.type == SPOT)
		{
			lightData.direction = glm::vec4(light.direction, 1.0f);
			lightData.innerCutoff = glm::cos(glm::radians(light.innerCutoff));
			lightData.outerCutoff = glm::cos(glm::radians(light.outerCutoff));
		}

		if (light.type == DIRECTIONAL)
		{
			lightData.position = glm::vec4(transform.position, 0.0f);

		}
		else
		{
			lightData.position = glm::vec4(transform.position + light.offset, 1.0f);
		}
		lightData.type = (float)light.type;
		lightData.color = glm::vec4(light.color * light.strength, 1.0f);
		lightData.radius = light.radius;
		lightData.shadowCaster = light.shadowCaster;

		EntityToLightMap[entity] = lightData;
		EntityToLightIndexMap[entity] = mLightIndex - 1;

		glNamedBufferSubData(mSsbo, EntityToLightIndexMap[entity] * sizeof(Light), sizeof(Light), (const void*)&lightData);
	}
}

void LightSystem::Update(float deltaTime)
{
	
	// delete lights 
	if (mEntities.size() < mLightIndex)
	{
		std::vector<Entity> entitiesDeleted{};

		for (const auto& pair : EntityToLightIndexMap)
		{
			if (!mEntities.contains(pair.first))
			{
				entitiesDeleted.push_back(pair.first);
				//std::cout << "Light deleted :" << pair.first << "\n";
			}
		}

		for (Entity deletedEntity : entitiesDeleted)
		{
			// replace the deleted light with an empty light struct in the ssbo.
			Light emptyLight;
			int indexOfDeletedLight = EntityToLightIndexMap[deletedEntity];
			glNamedBufferSubData(mSsbo, indexOfDeletedLight * sizeof(Light), sizeof(Light), (const void*)&emptyLight);

			// shift lights that had an index greater than the deleted light backwards.

			std::cout << "Entity to light index map before shift and deletion: " << "\n";
			for (auto& pair : EntityToLightIndexMap)
			{
				std::cout << "Entity: " << pair.first << " index: " << pair.second << "\n";
			}

			for (Entity e : mEntities)
			{
				if (EntityToLightIndexMap[e] > indexOfDeletedLight)
				{
					EntityToLightIndexMap[e] -= 1;
				}
			}

			EntityToLightIndexMap.erase(EntityToLightIndexMap.find(deletedEntity));
			EntityToLightMap.erase(EntityToLightMap.find(deletedEntity));
			mLightIndex--;

			std::cout << "Entity to light index map after shift and deletion: " << "\n";
			for (auto& pair : EntityToLightIndexMap)
			{
				std::cout << "Entity: " << pair.first << " index: " << pair.second << "\n";
			}

			for (Entity entity : mEntities)
			{
				auto& light = ecs.GetComponent<CLight>(entity);
				light.isDirty = true;
			}
		}
	}

	// add new lights
	if (mEntities.size() > mLightIndex)
	{
		mLightIndex++;

		for (Entity e : mEntities)
		{
			std::cout << "light entity: " << e << "\n";
		}

		std::vector<Entity> entitiesAdded;
		for (const auto& entity : mEntities)
		{
			if (!EntityToLightIndexMap.contains(entity))
			{
				entitiesAdded.push_back(entity);
				std::cout << "new light entity: " << entity << "\n";

				break;
			}
		}

		for (Entity addedEntity : entitiesAdded)
		{
			const auto& transform = ecs.GetComponent<CTransform>(addedEntity);
			const auto& light = ecs.GetComponent<CLight>(addedEntity);

			std::cout << "light type: " << light.type << "\n";

			if (light.type == SPOT)
			{
				lightData.direction = glm::vec4(light.direction, 1.0f);
				lightData.innerCutoff = glm::cos(glm::radians(light.innerCutoff));
				lightData.outerCutoff = glm::cos(glm::radians(light.outerCutoff));
			}

			lightData.type = (float)light.type;
			lightData.position = glm::vec4(transform.position + light.offset, 1.0f);
			lightData.color = glm::vec4(light.color * light.strength, 1.0f);
			lightData.radius = light.radius;

			EntityToLightMap[addedEntity] = lightData;
			EntityToLightIndexMap[addedEntity] = mLightIndex - 1;

			glNamedBufferSubData(mSsbo, EntityToLightIndexMap[addedEntity] * sizeof(Light), sizeof(Light), (const void*)&lightData);
		}
	}

	// update already existing lights
	// Here we fetch the light data for each entity and update it.
	// We need this to send the updated light data to the ssbo.
	for (const auto& entity : mEntities) 
	{
		auto& light = ecs.GetComponent<CLight>(entity);

		if (!EntityToLightIndexMap.contains(entity))
		{
			continue;
		}

		if (light.isDirty) 
		{	
			auto& transform = ecs.GetComponent<CTransform>(entity);

			if (light.type == SPOT)
			{
				EntityToLightMap[entity].direction = glm::vec4(light.direction, 1.0f);
				EntityToLightMap[entity].innerCutoff = glm::cos(glm::radians(light.innerCutoff));
				EntityToLightMap[entity].outerCutoff = glm::cos(glm::radians(light.outerCutoff));
			}
			if (light.type == DIRECTIONAL)
			{
				EntityToLightMap[entity].position = glm::vec4(transform.position, 0.0f);
			}
			else
			{
				EntityToLightMap[entity].position = glm::vec4(transform.position + light.offset, 1.0f);
				
			}

			EntityToLightMap[entity].color = glm::vec4(light.color * light.strength, 1.0f);
			EntityToLightMap[entity].radius = light.radius;	
			EntityToLightMap[entity].shadowCaster = light.shadowCaster;
			
			glNamedBufferSubData(mSsbo, EntityToLightIndexMap[entity] * sizeof(Light), sizeof(Light), (const void*)&EntityToLightMap[entity]);

			light.isDirty = false;
		}
	}	
}
