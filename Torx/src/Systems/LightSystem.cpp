#include "LightSystem.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CLight.h"
#include "glad/glad.h"
#include "iostream"
#include "../Core/Common.hpp"
#include <unordered_map>

extern Coordinator ecs;

struct Light
{
	glm::vec4 type;
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 quadratic;

	// for spotlight
	glm::vec4 direction;
	glm::vec4 innerCutoff;
	glm::vec4 outerCutoff;
};

std::vector<Light> lights;
std::unordered_map<Entity, int> EntityToLightMap;

void LightSystem::Init() 
{
	mMaxLights = 30;
	mLightIndex = 0;

	// create ssbo for storing lights
	glCreateBuffers(1, &mSsbo);
	glNamedBufferStorage(mSsbo, sizeof(Light) * mMaxLights, (const void*) nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mSsbo);

	lights.reserve(30);
}

void LightSystem::Update(float deltaTime, Camera& camera)
{

	//std::cout << "Camera position" << camera.Position.x << " " << camera.Position.y << " " << camera.Position.z << "\n";

	Light lightData;

	lights.clear();

	for (const auto& entity : mEntities) 
	{

		// if number of lights did not change only update position
		if (mEntities.size() <= mLightIndex)
		{
		//	/*const auto& transform = ecs.GetComponent<CTransform>(entity);
		//	Light currentLight = lights[EntityToLightMap[entity]];

		//	if ()
		//	currentLight.position = glm::vec4(transform.position, 1.0);*/

			return;
		}

		const auto& transform = ecs.GetComponent<CTransform>(entity);
		const auto& light = ecs.GetComponent<CLight>(entity);

		if (light.type == DIRECTIONAL)
		{
			lightData.type = glm::vec4(0.0f);
			lightData.position = glm::vec4(transform.position, 1.0f);
		}
		else if (light.type == POINT)
		{
			std::cout << "what \n";
			lightData.type = glm::vec4(1.0f);
			lightData.position = glm::vec4(transform.position, 1.0f);
		}
		else if (light.type == SPOT)
		{

			lightData.type = glm::vec4(2.0f);
			lightData.direction = glm::vec4(light.direction, 1.0f);
			lightData.innerCutoff = glm::vec4(glm::cos(glm::radians(light.innerCutoff)));
			lightData.outerCutoff = glm::vec4(glm::cos(glm::radians(light.outerCutoff)));
			lightData.position = glm::vec4(transform.position, 1.0f);

		}
		else if (light.type == FLASHLIGHT)
		{
			lightData.type = glm::vec4(2.0f);
			lightData.position = glm::vec4(camera.Position, 1.0f);

			std::cout << "Light data position" << lightData.position.x << " " << lightData.position.y << " " << lightData.position.z << "\n";
			lightData.direction = glm::vec4(camera.Front, 1.0f);
			lightData.innerCutoff = glm::vec4(glm::cos(glm::radians(light.innerCutoff)));
			lightData.outerCutoff = glm::vec4(glm::cos(glm::radians(light.outerCutoff)));
		}

		
		lightData.ambient = glm::vec4(light.ambient, 1.0f);
		lightData.diffuse = glm::vec4(light.diffuse, 1.0f);
		lightData.specular = glm::vec4(light.specular, 1.0f);
		lightData.quadratic = glm::vec4(light.quadratic);

		lights.push_back(lightData);

		EntityToLightMap[entity] = mLightIndex;
		mLightIndex++;
	}

	glNamedBufferSubData(mSsbo, 0, sizeof(Light) * lights.size(), (const void*) lights.data());
}