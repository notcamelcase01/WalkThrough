#pragma once
#include<string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Buffer.h"
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_Renderer_ID;
	std::unordered_map<const char*, GLint> m_AttribLocationCache;
	std::unordered_map<const char*, GLint> m_UniformLocationCache;

	GLint GetUniformLocation(const char* name);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexshader, const std::string& fragmentshader);
	ShaderProgramSource ParseShader(const std::string& filepath);

public:
	Shader(const std::string& filepath);
	~Shader();


	void Bind();
	void Unbind();
	GLint getAttribLocation(const char* name);
	void setUniform1i(const char* name, int i0);
	void setUniform1f(const char* name, float v0);
	void setUniform4fv(const char* name, glm::mat4 matrix);
	void setUniform3fv(const char* name, glm::vec3 vector3f);
};