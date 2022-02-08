#pragma once
#include <GLTypes/Shader.h>
#include <Utils/FileUtils.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char *vtxPath, const char *fragPath)
{
	// 1. read code from file
	std::string vtxShaderCodeStr = FileUtils::readFile(vtxPath);
	std::string fragShaderCodeStr = FileUtils::readFile(fragPath);

	const char *vtxShaderCode = vtxShaderCodeStr.c_str();
	const char *fragShaderCode = fragShaderCodeStr.c_str();

	// 2. compile shaders

	uint32_t vtx, frag;
	int32_t success;
	char infoLog[512];

	vtx = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vtx, 1, &vtxShaderCode, NULL);
	glCompileShader(vtx);

	glGetShaderiv(vtx, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(vtx, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	frag = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag, 1, &fragShaderCode, NULL);
	glCompileShader(frag);

	glGetShaderiv(frag, GL_COMPILE_STATUS, &success);

	if(!success)
	{
		glGetShaderInfoLog(frag, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	id = glCreateProgram();
	glAttachShader(id, vtx);
	glAttachShader(id, frag);
	glLinkProgram(id);

	glGetProgramiv(id, GL_LINK_STATUS, &success);

	if(!success)
	{
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << infoLog << std::endl;
	}

	glDeleteShader(vtx);
	glDeleteShader(frag);
	use();
}

int32_t Shader::getUniformLocation(const std::string &name)
{
	if(UniformLocations.find(name) != UniformLocations.end())
		return UniformLocations[name];
	int32_t loc = glGetUniformLocation(id, name.c_str());
	if(loc == -1)
		std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
	UniformLocations[name] = loc;
	return loc;
}
template<>
void Shader::setUniform(const std::string &name, const bool &value)
{
	useIfNecessary();
	glUniform1i(getUniformLocation(name), (int)value);
}template<>
void Shader::setUniform(const std::string &name, const int &value)
{
	useIfNecessary();
	glUniform1i(getUniformLocation(name), value);
}
template<>
void Shader::setUniform(const std::string &name, const glm::vec1 &val)
{
	useIfNecessary();
	glUniform1f(getUniformLocation(name), val.x);
}
template<>
void Shader::setUniform(const std::string &name, const glm::vec2 &val)
{
	useIfNecessary();
	glUniform2f(getUniformLocation(name), val.x, val.y);
}
template<>
void Shader::setUniform(const std::string &name, const glm::vec3 &val)
{
	useIfNecessary();
	glUniform3f(getUniformLocation(name), val.x, val.y, val.z);
}
template<>
void Shader::setUniform(const std::string &name, const glm::vec4 &val)
{
	useIfNecessary();
	glUniform4f(getUniformLocation(name), val.x, val.y, val.z, val.w);
}
template<>
void Shader::setUniform(const std::string &name, const glm::mat4 &val)
{
	useIfNecessary();
	glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(val));
}
uint32_t Shader::addTexture(const std::string &name, Texture2D *texture, int32_t id)
{
	useIfNecessary();
	uint32_t loc = getUniformLocation(name);
	if(id == -1)
	{
		for(uint32_t i = 0; i < 32; i++)
		{
			if(textures.find(i) == textures.end())
			{
				id = i;
				setUniform(name, id);
				break;
			}
		}
	}
	if(id == -1)
	{
		throw "Reached max texture count.";
		return id;
	}
	textures[id] = texture;
	glActiveTexture(GL_TEXTURE0 + id);
	texture->use();
	return id;
}

Texture2D *Shader::getTexture(const uint32_t textureId)
{
	if(textures.find(id) == textures.end())
		return nullptr;
	return textures[id];
}

void Shader::useTextures()
{
	useIfNecessary();
	std::unordered_map<uint32_t, Texture2D *>::iterator it;
	for(it = textures.begin(); it != textures.end(); it++)
	{
		glActiveTexture(GL_TEXTURE0 + it->first);
		it->second->use();
	}
}

void Shader::use()
{
	glUseProgram(id);
	lastUsed = id;
	useTextures();
}
void Shader::free()
{
	glDeleteProgram(id);
	release();
}
void Shader::release()
{
	lastUsed = 0;
	glUseProgram(0);
}

GLOBJ_LAST_USED(Shader)

IShaderUniform::IShaderUniform(const std::string &name) : name(name)
{
}


const std::string &IShaderUniform::getName() const
{
	return name;
}

template<typename Type>
ShaderUniform<Type>::ShaderUniform(const std::string &name, Type val) : IShaderUniform(name), val(val)
{
}

template <typename Type>
void ShaderUniform<Type>::updateShader(Shader &shader) const
{
	shader.setUniform<Type>(getName(), val);
}

template <typename Type>
void ShaderUniform<Type>::setValue(Shader &shader, Type val)
{
	this->val = val;
	updateShader(shader);
}

template <typename Type>
const Type &ShaderUniform<Type>::getValue() const
{
	return val;
}

template ShaderUniform<bool>;
template ShaderUniform<int>;
template ShaderUniform<glm::vec1>;
template ShaderUniform<glm::vec2>;
template ShaderUniform<glm::vec3>;
template ShaderUniform<glm::vec4>;
template ShaderUniform<glm::mat4>;