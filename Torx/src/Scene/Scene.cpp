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
#include "../Components/CCamera.h"
#include "../Components/CNativeScript.h"
#include "../Util/Util.h"
#include "../AssetLoading/AssetManager.h"
#include "../Util/RenderingUtil.h"
#include <algorithm>

extern Coordinator ecs;

namespace Scene
{
	glm::vec2 jsonToVec2(nlohmann::json json);
	glm::vec3 jsonToVec3(nlohmann::json json);
	glm::quat jsonToQuat(nlohmann::json json);
	glm::mat4 jsonToMat4(const nlohmann::json& jsonMat);
	nlohmann::json mat4ToJson(const glm::mat4& mat);

	nlohmann::json SerializeScene()
	{
		nlohmann::json json;

		std::vector<Entity> livingEntities = ecs.GetLivingEntities();
		for (Entity entity : livingEntities)
		{
			nlohmann::json e;


			bool hasComponents = false;

			//e["components"] = {};

			if (ecs.HasComponent<CTransform>(entity))
			{
				const auto& transform = ecs.GetComponent<CTransform>(entity);
				e["components"]["transform"]["position"] = { transform.position.x, transform.position.y, transform.position.z };
				e["components"]["transform"]["scale"] = { transform.scale.x, transform.scale.y, transform.scale.z };
				e["components"]["transform"]["rotation"] = { transform.rotation.w, transform.rotation.x, transform.rotation.y, transform.rotation.z };
				e["id"] = entity;

				hasComponents = true;
			}

			if (ecs.HasComponent<CRigidBody>(entity))
			{
				const auto& rigidBody = ecs.GetComponent<CRigidBody>(entity);
				e["components"]["rigidbody"]["mass"] = rigidBody.mass;

				e["id"] = entity;

				hasComponents = true;
			}


			if (ecs.HasComponent<CModel>(entity))
			{
				const auto& model = ecs.GetComponent<CModel>(entity);
				e["components"]["model"]["modelName"] = model.modelName;
				e["components"]["model"]["path"] = model.path;
				e["components"]["model"]["hasAOTexture"] = model.hasAOTexture;
				e["id"] = entity;

				hasComponents = true;
			}

			if (ecs.HasComponent<CMesh>(entity))
			{
				const auto& mesh = ecs.GetComponent<CMesh>(entity);
				e["components"]["mesh"]["meshType"] = mesh.meshType;
				if (mesh.texture == "")
				{
					e["components"]["mesh"]["texture"] = "whitepaper";
				}
				else
				{
					e["components"]["mesh"]["texture"] = mesh.texture;
				}
				e["components"]["mesh"]["textureScaling"] = { mesh.textureScaling.x, mesh.textureScaling.y };
				e["id"] = entity;

				hasComponents = true;
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

				hasComponents = true;
			}

			if (ecs.HasComponent<CPlayer>(entity))
			{
				const auto& player = ecs.GetComponent<CPlayer>(entity);

				e["components"]["player"]["flashlightOn"] = player.flashlightOn;
				e["components"]["player"]["movementSpeed"] = player.movementSpeed;

				hasComponents = true;
			}

			if (ecs.HasComponent<CCamera>(entity))
			{
				const auto& camera = ecs.GetComponent<CCamera>(entity);

				if (camera.projType == PERSPECTIVE)
				{
					e["components"]["camera"]["projType"] = "perspective";
				}
				else if (camera.projType == ORTHO)
				{
					e["components"]["camera"]["projType"] = "orthographic";
				}

				e["components"]["camera"]["projection"] = mat4ToJson(camera.projection);
				e["components"]["camera"]["fov"] = camera.fov;
				e["components"]["camera"]["near"] = camera.near;
				e["components"]["camera"]["far"] = camera.far;
				e["components"]["camera"]["left"] = camera.left;
				e["components"]["camera"]["right"] = camera.right;
				e["components"]["camera"]["bottom"] = camera.bottom;
				e["components"]["camera"]["top"] = camera.top;

				hasComponents = true;
			}

			if (ecs.HasComponent<CNativeScript>(entity))
			{
				const auto& scriptComponent = ecs.GetComponent<CNativeScript>(entity);

				e["components"]["script"]["name"] = scriptComponent.name;

				hasComponents = true;
			}

			
			json["entities"].push_back(e);
		}

		json["config"]["environmentMap"] = environmentMap;

		// VXGI
		json["config"]["vxgi"]["vxgi"] = Common::vxgi;
		json["config"]["vxgi"]["voxelGridDimensions"] = Common::voxelGridDimensions;
		json["config"]["vxgi"]["diffuseConeSpread"] = Common::diffuseConeSpread;
		json["config"]["vxgi"]["voxelizationAreaSize"] = Common::voxelizationAreaSize;
		json["config"]["vxgi"]["vxSpecularBias"] = Common::vxSpecularBias;
		json["config"]["vxgi"]["specularStepSizeMultiplier"] = Common::specularStepSizeMultiplier;
		json["config"]["vxgi"]["specularConeOriginOffset"] = Common::specularConeOriginOffset;
		json["config"]["vxgi"]["specularConeMaxDistance"] = Common::specularConeMaxDistance;

		// SSR
		json["config"]["ssr"]["ssrMaxDistance"] = Common::ssrMaxDistance;
		json["config"]["ssr"]["ssrResolution"] = Common::ssrResolution;
		json["config"]["ssr"]["ssrSteps"] = Common::ssrSteps;
		json["config"]["ssr"]["ssrThickness"] = Common::ssrThickness;
		json["config"]["ssr"]["ssrSpecularBias"] = Common::ssrSpecularBias;
		json["config"]["ssr"]["ssrMaxBlurDistance"] = Common::ssrMaxBlurDistance;

		// SSAO
		json["config"]["ssao"]["ssaoRadius"] = Common::ssaoRadius;
		json["config"]["ssao"]["ssaoPower"] = Common::ssaoPower;
		json["config"]["ssao"]["ssaoKernelSize"] = Common::ssaoKernelSize;
		json["config"]["ssao"]["ssaoOn"] = Common::ssaoOn;

		return json;
	}

	void DeserializeScene(nlohmann::json& jsonData)
	{
		for (const auto& e : jsonData["entities"])
		{
			Entity newEntity = ecs.CreateEntity();
			
			if (!e.contains("components"))
				break;
		
			if (e["components"].contains("transform"))
			{
				ecs.AddComponent<CTransform>(
					newEntity,
					CTransform{
						.position = jsonToVec3(e["components"]["transform"]["position"]),
						.scale = jsonToVec3(e["components"]["transform"]["scale"]),
						.rotation = jsonToQuat(e["components"]["transform"]["rotation"]),
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

				std::string name = e["components"]["model"]["modelName"].get<std::string>();
				std::string path = e["components"]["model"]["path"].get<std::string>();
				
				std::cout << "model path: " << path << "\n";
				AssetManager::LoadModel(path, name);	
					
				ecs.AddComponent<CModel>(
					newEntity,
					CModel{
						.model = AssetManager::GetModel(name),
						.modelName = name,
						.path = path,
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
						.shadowCaster = e["components"]["light"]["shadowCaster"].get<bool>(),
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

			if (e["components"].contains("camera"))
			{
				ProjType cameraProjType;
				if (e["components"]["camera"]["projType"] == "perspective")
				{
					cameraProjType = PERSPECTIVE;
				}
				else if (e["components"]["camera"]["projType"] == "orthographic")
				{
					cameraProjType = ORTHO;
				}

				ecs.AddComponent<CCamera>(
					newEntity,
					CCamera{
						.projType = cameraProjType,
						.projection = jsonToMat4(e["components"]["camera"]["projection"]),
						.fov = e["components"]["camera"]["fov"],
						.near = e["components"]["camera"]["near"],
						.far = e["components"]["camera"]["far"],
						.left = e["components"]["camera"]["left"],
						.right = e["components"]["camera"]["right"],
						.bottom = e["components"]["camera"]["bottom"],
						.top = e["components"]["camera"]["top"],
					});
			}

			if (e["components"].contains("script"))
			{
				ecs.AddComponent<CNativeScript>(
					newEntity,
					CNativeScript{
						.name = e["components"]["script"]["name"].get<std::string>(),
					});
			}
		}

		//std::string envMapPath = "res/textures/hdr/" + jsonData["config"]["environmentMap"].get<std::string>();
		SetEnvironmentMap(jsonData["config"]["environmentMap"].get<std::string>());
		RenderingUtil::LoadNewEnvironmentMap(jsonData["config"]["environmentMap"].get<std::string>().c_str());

		// VXGI
		if (jsonData["config"].contains("vxgi"))
		{
			Common::vxgi = jsonData["config"]["vxgi"]["vxgi"].get<bool>();
			Common::voxelGridDimensions = jsonData["config"]["vxgi"]["voxelGridDimensions"].get<int>();
			Common::diffuseConeSpread = jsonData["config"]["vxgi"]["diffuseConeSpread"].get<float>();
			Common::voxelizationAreaSize = jsonData["config"]["vxgi"]["voxelizationAreaSize"].get<float>();
			Common::vxSpecularBias = jsonData["config"]["vxgi"]["vxSpecularBias"].get<float>();
			Common::specularStepSizeMultiplier = jsonData["config"]["vxgi"]["specularStepSizeMultiplier"].get<float>();
			Common::specularConeOriginOffset = jsonData["config"]["vxgi"]["specularConeOriginOffset"].get<float>();
			Common::specularConeMaxDistance = jsonData["config"]["vxgi"]["specularConeMaxDistance"].get<float>();
		}

		// SSR
		if (jsonData["config"].contains("ssr"))
		{
			Common::ssrMaxDistance = jsonData["config"]["ssr"]["ssrMaxDistance"].get<float>();
			Common::ssrResolution = jsonData["config"]["ssr"]["ssrResolution"].get<float>();
			Common::ssrSteps = jsonData["config"]["ssr"]["ssrSteps"].get<int>();
			Common::ssrThickness = jsonData["config"]["ssr"]["ssrThickness"].get<float>();
			Common::ssrSpecularBias = jsonData["config"]["ssr"]["ssrSpecularBias"].get<float>();
			Common::ssrMaxBlurDistance = jsonData["config"]["ssr"]["ssrMaxBlurDistance"].get<float>();
		}

		// SSAO
		if (jsonData["config"].contains("ssao"))
		{
			Common::ssaoRadius = jsonData["config"]["ssao"]["ssaoRadius"].get<float>();;
			Common::ssaoPower = jsonData["config"]["ssao"]["ssaoPower"].get<float>();;
			Common::ssaoKernelSize = jsonData["config"]["ssao"]["ssaoKernelSize"].get<int>();;
			Common::ssaoOn = jsonData["config"]["ssao"]["ssaoOn"].get<bool>();
		}
	}

	nlohmann::json SaveSceneToJson(const std::string& path)
	{
		nlohmann::json serializedScene = SerializeScene();

		std::ofstream o(path);
		if (!o.is_open())
		{
			std::cout << "Failed to open json file.\n";
		}
		else
		{
			o << serializedScene.dump(4);
			o.close();
			g_currentScenePath = path;
		}

		return serializedScene;
	}

	void LoadSceneFromJson(const std::string& path)
	{
		std::cout << "Loading scene..." << "\n";

		std::ifstream f(path);
		if (!f.is_open())
		{
			std::cout << "Failed to open json file. \n";

			return;
		}
		nlohmann::json jsonData;
		f >> jsonData;
		f.close();

		DeserializeScene(jsonData);

		g_currentScenePath = path;

		std::cout << "scene loaded " << "\n";
	}

	void SetEnvironmentMap(const std::string& envMap)
	{
		environmentMap = envMap;
	}

	glm::quat jsonToQuat(nlohmann::json json)
	{
		std::vector<float> rot = json.get<std::vector<float>>();

		return glm::quat(rot[0], rot[1], rot[2], rot[3]);
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

	glm::mat4 jsonToMat4(const nlohmann::json& jsonMat) {
		glm::mat4 mat(1.0f); // Default to identity
		for (int i = 0; i < 16; ++i) {
			glm::value_ptr(mat)[i] = jsonMat[i];
		}
		return mat;
	}

	nlohmann::json mat4ToJson(const glm::mat4& mat) {
		nlohmann::json jsonMat;
		for (int i = 0; i < 16; ++i) {
			jsonMat.push_back(glm::value_ptr(mat)[i]);
		}
		return jsonMat;
	}
}



