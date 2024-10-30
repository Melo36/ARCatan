#pragma once

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"

struct ShaderProgramSource {
	std::string vertexSource;
	std::string fragmentSource;
};

class Shader
{
private:
	std::string m_filePath;
	unsigned int m_rendererID;
	std::unordered_map<std::string, int> m_uniformLocationCache;
public:
	Shader(const std::string& filePath);
	~Shader();

	void bind() const;
	void unBind() const;

	//Set uniforms
	void setUniform1i(const std::string& name, int value);
	void setUniform1f(const std::string& name, float value);
	void setUniform3f(const std::string& name, glm::vec3 v);
	void setUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void setUniformMat3f(const std::string& name, const glm::mat3& matrix);
	void setUniformMat4f(const std::string& name, const glm::mat4& matrix);
private:
	ShaderProgramSource parseShader(const std::string& filepath);
	unsigned int compileShader(unsigned int type, const std::string& source);
	unsigned int createShaders(const std::string& vertexShader, const std::string& fragmentShader);
	int getUniformLocation(const std::string& name);
};