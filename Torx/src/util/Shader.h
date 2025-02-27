#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
public:
	unsigned int ID;

	Shader();

	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec3(const std::string& name, float value1, float value2, float value3) const;
	void setVec2(const std::string& name, glm::vec2 value) const;
	void setVec2(const std::string& name, float value1, float value2) const;
	void setVec4(const std::string& name, glm::vec4 value) const;
	void setVec4(const std::string& name, float value1, float value2, float value3, float value4) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
};
