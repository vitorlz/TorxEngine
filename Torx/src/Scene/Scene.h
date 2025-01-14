#pragma once 

namespace Scene
{
	void LoadSceneFromJson(const std::string& filename);
	void SaveSceneToJson(const std::string& filename);
	void SetEnvironmentMap(const std::string& envMap);

	inline std::string g_currentScenePath = std::string();
};