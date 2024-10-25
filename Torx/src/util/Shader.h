#pragma once

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Shader
{
public:
	// the program ID
	unsigned int ID;

	// constructor reads and builds the shader
	Shader(const char* vertexPath, const char* fragmentPath);
	Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath);

	// use/activate the shader
	void use();

	// utility uniform functions
	// the first const makes sure that the original variable referred to by the reference name is
	// not modified by the function. The second const makes sure that the function won't modify
	// any of the member variables of the class (does not modify the state of the class). So if we
	// make a const instance of the class we can only use const functions.
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec3(const std::string& name, float value1, float value2, float value3) const;
	void setVec4(const std::string& name, glm::vec4 value) const;
	void setVec4(const std::string& name, float value1, float value2, float value3, float value4) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
};
