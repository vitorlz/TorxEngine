#include <nlohmann/json.hpp>
#include "Scene.h"
#include "../Core/Coordinator.hpp"
#include "../Core/Types.hpp"
#include <vector>
#include <fstream>
#include "../Components/CTransform.h"
#include "../Components/CLight.h"
#include "../Components/CModel.h"
#include "../Components/CSingleton_Input.h"
#include "../Components/CPlayer.h"
#include "../Components/CRigidBody.h"
#include "../Components/CMesh.h"
#include "../Util/Util.h"
#include "../AssetLoading/AssetManager.h"

extern Coordinator ecs;

glm::vec2 jsonToVec2(nlohmann::json json);
glm::vec3 jsonToVec3(nlohmann::json json);

void Scene::SaveSceneToJson(const std::string& filename)
{
	std::vector<Entity> livingEntities = ecs.GetLivingEntities();
	nlohmann::json entities;

	for (Entity entity : livingEntities)
	{
		nlohmann::json e;
		
		if (ecs.HasComponent<CTransform>(entity)) 
		{
			const auto& transform = ecs.GetComponent<CTransform>(entity);
			e["components"]["transform"]["position"] = { transform.position.x, transform.position.y, transform.position.z };
			e["components"]["transform"]["scale"] = { transform.scale.x, transform.scale.y, transform.scale.z };
			e["components"]["transform"]["rotation"] = { transform.rotation.x, transform.rotation.y, transform.rotation.z };
			e["id"] = entity;
		}

		if (ecs.HasComponent<CRigidBody>(entity))
		{
			const auto& rigidBody = ecs.GetComponent<CRigidBody>(entity);
			e["components"]["rigidbody"]["mass"] = rigidBody.mass;
		
			e["id"] = entity;
		}


		if (ecs.HasComponent<CModel>(entity))
		{
			const auto& model = ecs.GetComponent<CModel>(entity);
			e["components"]["model"]["modelName"] = model.modelName;
			e["components"]["model"]["hasAOTexture"] = model.hasAOTexture;
			e["id"] = entity;
		}
		
		if (ecs.HasComponent<CMesh>(entity))
		{
			const auto& mesh = ecs.GetComponent<CMesh>(entity);
			e["components"]["mesh"]["meshType"] = mesh.meshType;
			e["components"]["mesh"]["texture"] = mesh.texture;
			e["components"]["mesh"]["textureScaling"] = { mesh.textureScaling.x, mesh.textureScaling.y };
			e["id"] = entity;
		}

		if (ecs.HasComponent<CLight>(entity))
		{
			auto& light = ecs.GetComponent<CLight>(entity);

			e["components"]["light"]["type"] = light.type;
			e["components"]["light"]["color"] = { light.color.x, light.color.y, light.color.z };
			e["components"]["light"]["radius"] = light.radius;
			e["components"]["light"]["strength"] = light.strength;
			e["components"]["light"]["direction"] = { light.direction.x, light.direction.y, light.direction.z };
			e["components"]["light"]["innerCutoff"] = light.innerCutoff;
			e["components"]["light"]["outerCutoff"] = light.outerCutoff;
			e["components"]["light"]["shadowCaster"] = light.shadowCaster;
			e["components"]["light"]["isDirty"] = light.isDirty;
			e["components"]["light"]["offset"] = { light.offset.x , light.offset.y, light.offset.z };
		}

		if (ecs.HasComponent<CPlayer>(entity))
		{
			const auto& player = ecs.GetComponent<CPlayer>(entity);

			e["components"]["player"]["flashlightOn"] = player.flashlightOn;
			e["components"]["player"]["movementSpeed"] = player.movementSpeed;
		}

		entities.push_back(e);
	}

	std::ofstream o("res/scenes/" + filename);
	if (!o.is_open())
	{
		std::cout << "Failed to open json file.\n";
	}
	else
	{
		o << entities.dump(4);
		o.close();
	}
}


void Scene::LoadSceneFromJson(const std::string& filename)
{
	std::cout << "Loading scene..." << "\n";

	std::ifstream f("res/scenes/" + filename);
	if (!f.is_open())
	{
		std::cout << "Failed to open json file. \n";

		return;
	}
	nlohmann::json jsonData; 
	f >> jsonData;
	f.close();

	for (const auto& e : jsonData)
	{
		Entity newEntity = ecs.CreateEntity();

		if (e["components"].contains("transform"))
		{	

			ecs.AddComponent<CTransform>(
				newEntity,
				CTransform{
					.position = jsonToVec3(e["components"]["transform"]["position"]),
					.scale = jsonToVec3(e["components"]["transform"]["scale"]),
					.rotation = jsonToVec3(e["components"]["transform"]["rotation"]),
				});
		}

		if (e["components"].contains("rigidbody"))
		{

			std::cout << "mass: " << e["components"]["rigidbody"]["mass"].get<btScalar>() << "\n";

			ecs.AddComponent<CRigidBody>(
				newEntity,
				CRigidBody{
					.mass = e["components"]["rigidbody"]["mass"].get<btScalar>(),
				});
		}

		if (e["components"].contains("model"))
		{

			std::cout << "model name: " << e["components"]["model"]["modelName"].get<std::string>() << "\n";;

			ecs.AddComponent<CModel>(
				newEntity,
				CModel{
					.model = AssetManager::GetModel(e["components"]["model"]["modelName"].get<std::string>()),
					.modelName = e["components"]["model"]["modelName"].get<std::string>(),
					.hasAOTexture = e["components"]["model"]["hasAOTexture"].get<bool>()
				});
		}

		if (e["components"].contains("mesh"))
		{

			std::cout << "mesh type: " << e["components"]["mesh"]["meshType"].get<std::string>() << "\n";

			ecs.AddComponent<CMesh>(
				newEntity,
				CMesh{
					.mesh = AssetManager::GetMesh(e["components"]["mesh"]["meshType"].get<std::string>()),
					.meshType = e["components"]["mesh"]["meshType"].get<std::string>(),  
					.texture = e["components"]["mesh"]["texture"].get<std::string>(),
					.textureScaling = jsonToVec2(e["components"]["mesh"]["textureScaling"])
				});

			
			auto& meshComponent = ecs.GetComponent<CMesh>(newEntity);
			meshComponent.mesh.textures = AssetManager::LoadMeshTextures(meshComponent.texture.c_str());
		}

		if (e["components"].contains("light"))
		{
			ecs.AddComponent<CLight>(
				newEntity,
				CLight{
					.type = e["components"]["light"]["type"].get<LightType>(),
					.color = jsonToVec3(e["components"]["light"]["color"]),
					.radius = e["components"]["light"]["radius"].get<float>(),
					.strength = e["components"]["light"]["strength"].get<float>(),
					.direction = jsonToVec3(e["components"]["light"]["direction"]),
					.innerCutoff = e["components"]["light"]["innerCutoff"].get<float>(),
					.outerCutoff = e["components"]["light"]["outerCutoff"].get<float>(),
					.shadowCaster = e["components"]["light"]["shadowCaster"].get<int>(),
					.isDirty = true,
					.offset = jsonToVec3(e["components"]["light"]["offset"])
				});
			
		}

		if (e["components"].contains("player"))
		{
			ecs.AddComponent<CPlayer>(
				newEntity,
				CPlayer{
					.flashlightOn = e["components"]["player"]["flashlightOn"].get<bool>(),
					.movementSpeed = e["components"]["player"]["movementSpeed"].get<float>(),
				});
		}
	}

	std::cout << "scene loaded " << "\n";
}

glm::vec3 jsonToVec3(nlohmann::json json)
{
	std::vector<float> pos = json.get<std::vector<float>>();

	return glm::vec3(pos[0], pos[1], pos[2]);
}

glm::vec2 jsonToVec2(nlohmann::json json)
{
	std::vector<float> pos = json.get<std::vector<float>>();

	return glm::vec2(pos[0], pos[1]);
}