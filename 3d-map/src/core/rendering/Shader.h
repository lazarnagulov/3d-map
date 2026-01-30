#pragma once

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
	Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath);
	~Shader();
	void Bind() const;
	void Unbind() const;
	void SetUniform1i(const std::string& name, int value);
	void SetUniformMat4(const std::string& name, const glm::mat4& value);
	void SetUniform4f(const std::string& name, const glm::vec4& value);
	void SetUniformVec3(const std::string& name, const glm::vec3& value);
	void SetUniform1f(const std::string& name, float value);
private:
	unsigned int CompileShader(unsigned int type, const std::string& source) const;
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) const;
	int GetUniformLocation(const std::string& name);
	std::string ParseShader(const std::string& source) const;
private:
	unsigned int m_RendererId;
	std::string m_VertexFilePath;
	std::string m_FragmentFilePath;
	std::unordered_map<std::string, int> m_UniformLocationCache;
};