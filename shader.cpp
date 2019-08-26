#include "shader.h"





Shader::Shader(const std::string& filepath) : m_FilePath(filepath)
{
	ShaderProgramSource source = ParseShader(filepath);
	std::cout << source.VertexSource << source.FragmentSource << std::endl;
	m_Renderer_ID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_Renderer_ID);
}

void Shader::Bind()
{
	glUseProgram(m_Renderer_ID);
}

void Shader::Unbind()
{
	glUseProgram(0);

}

GLint Shader::getAttribLocation(const char* name)
{
	if (m_AttribLocationCache.find(name) != m_AttribLocationCache.end())
		return m_AttribLocationCache[name];
	GLint location = glGetAttribLocation(m_Renderer_ID, name);
	if (-1 == location) {
		std::cout << "WARNING: AttribLocation " << name << " do not exist" << std::endl;
	}
	m_UniformLocationCache[name] = location;
	return glGetAttribLocation(m_Renderer_ID, name);
}


GLint Shader::GetUniformLocation(const char* name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
		return m_UniformLocationCache[name];

	GLint location = glGetUniformLocation(m_Renderer_ID, name);
	if (-1 == location) {
		std::cout << "WARNING: Uniform " << name << " do not exist" << std::endl;
	}

	m_UniformLocationCache[name] = location;
	return location;
}

void Shader::setUniform1f(const char* name, float v0)
{
	glUniform1f(GetUniformLocation(name), v0);
}

void Shader::setUniform4fv(const char* name, glm::mat4 matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::setUniform3fv(const char* name, glm::vec3 vector3f)
{
	glUniform3fv(GetUniformLocation(name), 1, glm::value_ptr(vector3f));
}

void Shader::setUniform1i(const char* name, int i0)
{

	glUniform1i(GetUniformLocation(name), i0);
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// this below code is to see if the shader compiled properly
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (GL_FALSE == result)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length * sizeof(char)];
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << "failed to compile : " << (type == GL_VERTEX_SHADER ? "Vertex Shader" : "fragment shader") << std::endl;
		std::cout << message << std::endl;
		delete[]message;

		return 0;
	}
	//////


	return id;
}


unsigned int Shader::CreateShader(const std::string& vertexshader, const std::string& fragmentshader)
{
	unsigned int program = glCreateProgram();

	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexshader);     //vertex shader
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentshader);  // fragment shader

	glAttachShader(program, vs);
	glAttachShader(program, fs);

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;

}



ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, FRAGMENT = 1
	};

	std::string line;
	std::stringstream ss[2];
	ShaderType type = ShaderType::NONE;

	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}

		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return { ss[0].str(),ss[1].str() };
}