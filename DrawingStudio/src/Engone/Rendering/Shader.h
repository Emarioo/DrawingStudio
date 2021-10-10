#pragma once

#include "glm/glm.hpp"
#include <string>
#include <unordered_map>

struct ShaderProgramSource {
	std::string vert;
	std::string frag;
};

class Shader {
public:
	Shader()=default;
	Shader(const std::string& source);

	void Init(const std::string& source);
	
	void Bind();
	void SetInt(const std::string& name, int i);
	void SetFloat(const std::string& name, float f);
	void SetVec2(const std::string& name, glm::vec2);
	void SetVec2I(const std::string& name, glm::ivec2);
	void SetVec3(const std::string& name, glm::vec3);
	void SetVec3I(const std::string& name, glm::ivec3);
	void SetVec4(const std::string& name, float f0, float f1, float f2, float f3);
	void SetMatrix(const std::string& name, glm::mat4 v);
		
	std::string shaderPath;
private:
	unsigned int id;
	unsigned int CreateShader(const std::string& vert, const std::string& frag);
	unsigned int CompileShader(const unsigned int, const std::string& src);
	ShaderProgramSource ParseShader(const std::string& filepath);
	
	unsigned int GetUniformLocation(const std::string& name);
	char err;
	std::unordered_map<std::string, unsigned int> uniLocations;
	int section[3];
};