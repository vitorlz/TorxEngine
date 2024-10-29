#include "LightSystem.h"
#include "../Core/Coordinator.hpp"
#include "../Components/CTransform.h"
#include "../Components/CLight.h"
#include "glad/glad.h"
#include "iostream"

extern Coordinator ecs;

struct Light
{
	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;
	glm::vec4 quadratic;
};

std::vector<Light> lights;

void LightSystem::Init() 
{
	mMaxLights = 30;
	mLightNumber = 0;

	// create ssbo for storing lights
	glCreateBuffers(1, &mSsbo);
	glNamedBufferStorage(mSsbo, sizeof(Light) * mMaxLights, (const void*) nullptr, GL_DYNAMIC_STORAGE_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, mSsbo);

}

void LightSystem::Update(float deltaTime)
{

	Light lightData;

	lights.clear();

	for (const auto& entity : mEntities) 
	{

		if (mEntities.size() <= mLightNumber) 
		{
			return;
		}
		const auto& transform = ecs.GetComponent<CTransform>(entity);
		const auto& light = ecs.GetComponent<CLight>(entity);


		lightData.position = glm::vec4(transform.position, 1.0);
		lightData.ambient = glm::vec4(light.ambient, 1.0);
		lightData.diffuse = glm::vec4(light.diffuse, 1.0);
		lightData.specular = glm::vec4(light.specular, 1.0);
		lightData.quadratic = glm::vec4(light.quadratic);

		lights.push_back(lightData);

		mLightNumber++;
	}

	std::cout << lights.size() << "\n";
	std::cout << "lightPos: " << lights[0].position.x << " " << lights[0].position.y << " " << lights[0].position.z << "\n";
	std::cout << "light diffuse: " << lights[0].diffuse.x << " " << lights[0].diffuse.y << " " << lights[0].diffuse.z << "\n";

	glNamedBufferSubData(mSsbo, 0, sizeof(Light) * lights.size(), (const void*) lights.data());
}