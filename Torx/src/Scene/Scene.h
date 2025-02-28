#pragma once 

#include <string>
#include <nlohmann/json.hpp>

namespace Scene
{
	void LoadSceneFromJson(const std::string& filename);
	nlohmann::json SaveSceneToJson(const std::string& filename);
	void SetEnvironmentMap(const std::string& envMap);
	nlohmann::json SerializeScene();
	void DeserializeScene(nlohmann::json& json);

	inline std::string g_currentScenePath = std::string();
	inline nlohmann::json g_editorScene;
	inline std::string environmentMap;

};