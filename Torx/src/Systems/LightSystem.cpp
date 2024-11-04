#include "LightSystem.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CLight.h"
#include "glad/glad.h"
#include "iostream"
#include "../Core/Common.h"
#include <unordered_map>

extern Coordinator ecs;

struct Light
{
	glm::vec4 type;
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 radius;

	// for spotlight
	glm::vec4 direction;
	glm::vec4 innerCutoff;
	glm::vec4 outerCutoff;

	glm::vec4 shadowCaster;
	glm::vec4 isDirty;
	glm::vec4 dynamic;
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
			lightData.innerCutoff = glm::vec4(glm::cos(glm::radians(light.innerCutoff)));
			lightData.outerCutoff = glm::vec4(glm::cos(glm::radians(light.outerCutoff)));
		}

		lightData.type = glm::vec4((float)light.type);

		if (light.type == DIRECTIONAL)
		{
			lightData.position = glm::vec4(transform.position, 0.0f);
		}
		else
		{
			lightData.position = glm::vec4(transform.position, 1.0f);
		}
		lightData.ambient = glm::vec4(light.ambient, 1.0f);
		lightData.diffuse = glm::vec4(light.diffuse, 1.0f);
		lightData.specular = glm::vec4(light.specular, 1.0f);
		lightData.radius = glm::vec4(light.radius);
		lightData.shadowCaster = glm::vec4(light.shadowCaster);

		EntityToLightMap[entity] = lightData;
		EntityToLightIndexMap[entity] = mLightIndex - 1;

		glNamedBufferSubData(mSsbo, EntityToLightIndexMap[entity] * sizeof(Light), sizeof(Light), (const void*)&lightData);
	}
}

void LightSystem::Update(float deltaTime, Camera& camera)
{
	// Here we fetch the light data for each entity and update it.
	// We need this to send the updated light data to the ssbo. 

	for (const auto& entity : mEntities) 
	{
		
		auto& light = ecs.GetComponent<CLight>(entity);

		if (light.dynamic)
		{
			const auto& transform = ecs.GetComponent<CTransform>(entity);


			if (light.type == DIRECTIONAL)
			{
				lightData.position = glm::vec4(transform.position, 0.0f);
			}
			else if (light.type == SPOT)
			{
				EntityToLightMap[entity].position = glm::vec4(camera.Position, 1.0f);
				EntityToLightMap[entity].direction = glm::vec4(camera.Front, 1.0f);
				EntityToLightMap[entity].innerCutoff = glm::vec4(glm::cos(glm::radians(light.innerCutoff)));
				EntityToLightMap[entity].outerCutoff = glm::vec4(glm::cos(glm::radians(light.outerCutoff)));
			}
			else
			{
				lightData.position = glm::vec4(transform.position, 1.0f);
			}

			EntityToLightMap[entity].ambient = glm::vec4(light.ambient, 1.0f);
			EntityToLightMap[entity].diffuse = glm::vec4(light.diffuse, 1.0f);
			EntityToLightMap[entity].specular = glm::vec4(light.specular, 1.0f);
			EntityToLightMap[entity].radius = glm::vec4(light.radius);
			EntityToLightMap[entity].shadowCaster = glm::vec4(light.shadowCaster);

			glNamedBufferSubData(mSsbo, EntityToLightIndexMap[entity] * sizeof(Light), sizeof(Light), (const void*)&EntityToLightMap[entity]);
		}

		if (!light.dynamic && light.isDirty) 
		{	
			const auto& transform = ecs.GetComponent<CTransform>(entity);
			
			std::cout << "light is dirty \n";
			EntityToLightMap[entity].position = glm::vec4(transform.position, 1.0f);

			if (light.type == SPOT)
			{
				EntityToLightMap[entity].position = glm::vec4(camera.Position, 1.0f);
				EntityToLightMap[entity].direction = glm::vec4(camera.Front, 1.0f);
				EntityToLightMap[entity].innerCutoff = glm::vec4(glm::cos(glm::radians(light.innerCutoff)));
				EntityToLightMap[entity].outerCutoff = glm::vec4(glm::cos(glm::radians(light.outerCutoff)));
			}

			EntityToLightMap[entity].ambient = glm::vec4(light.ambient, 1.0f);
			EntityToLightMap[entity].diffuse = glm::vec4(light.diffuse, 1.0f);
			EntityToLightMap[entity].specular = glm::vec4(light.specular, 1.0f);
			EntityToLightMap[entity].radius = glm::vec4(light.radius);
			EntityToLightMap[entity].shadowCaster = glm::vec4(light.shadowCaster);

			glNamedBufferSubData(mSsbo, EntityToLightIndexMap[entity] * sizeof(Light), sizeof(Light), (const void*)&EntityToLightMap[entity]);

			light.isDirty = false;
		}
	}

	// check if any lights were deleted to update ssbo. If no lights were added or deleted, the number of entities will be equal to the lightIndex.
	if (mEntities.size() < mLightIndex)
	{
		Entity entityDeleted{};

		for (const auto& pair : EntityToLightIndexMap)
		{
			if (!mEntities.contains(pair.first))
			{
				entityDeleted = pair.first;
				std::cout << "Entity deleted :" << entityDeleted << "\n";
			}
		}

		Light emptyLight;

		glNamedBufferSubData(mSsbo, EntityToLightIndexMap[entityDeleted] * sizeof(Light), sizeof(Light), (const void*)&emptyLight);

		std::cout << "Light destroyed" << "\n";
		std::cout << "Light index before deletion: " << mLightIndex << "\n";
		std::cout << "Entities before deletion: " << mEntities.size() << "\n";

		EntityToLightIndexMap.erase(EntityToLightIndexMap.find(entityDeleted));
		EntityToLightMap.erase(EntityToLightMap.find(entityDeleted));
		mLightIndex--;

		std::cout << "Light index after deletion: " << mLightIndex << "\n";
		std::cout << "Entities after deletion: " << mEntities.size() << "\n";

	}

	// check if any lights were added and update ssbo

	if (mEntities.size() > mLightIndex)
	{
		std::cout << "Entities before addition: " << mEntities.size() << "\n";

		std::cout << "Light index before addition: " << mLightIndex << "\n";

		mLightIndex++;

		std::cout << "Light index after addition: " << mLightIndex << "\n";
		std::cout << "Entities after addition: " << mEntities.size() << "\n";

		Entity entityAdded{};
		for (const auto& entity : mEntities)
		{
			if (!EntityToLightIndexMap.contains(entity))
			{
				entityAdded = entity;
				std::cout << "Entity Added:" << entityAdded << "\n";
			}
		}

		const auto& transform = ecs.GetComponent<CTransform>(entityAdded);
		const auto& light = ecs.GetComponent<CLight>(entityAdded);

		std::cout << "light type: " << light.type << "\n";

		if (light.type == SPOT)
		{
			lightData.direction = glm::vec4(light.direction, 1.0f);
			lightData.innerCutoff = glm::vec4(glm::cos(glm::radians(light.innerCutoff)));
			lightData.outerCutoff = glm::vec4(glm::cos(glm::radians(light.outerCutoff)));
		}

		lightData.type = glm::vec4((float)light.type);
		lightData.position = glm::vec4(transform.position, 1.0f);
		lightData.ambient = glm::vec4(light.ambient, 1.0f);
		lightData.diffuse = glm::vec4(light.diffuse, 1.0f);
		lightData.specular = glm::vec4(light.specular, 1.0f);
		lightData.radius = glm::vec4(light.radius);

		EntityToLightMap[entityAdded] = lightData;
		EntityToLightIndexMap[entityAdded] = mLightIndex - 1;

		glNamedBufferSubData(mSsbo, EntityToLightIndexMap[entityAdded] * sizeof(Light), sizeof(Light), (const void*)&lightData);
	}
}