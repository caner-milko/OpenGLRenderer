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
void Shader::setUniform(const int32_t location, const bool &value)
{
	useIfNecessary();
	glUniform1i(location, (int)value);
}template<>
void Shader::setUniform(const int32_t location, const int &value)
{
	useIfNecessary();
	glUniform1i(location, value);
}
template<>
void Shader::setUniform(const int32_t location, const float &val)
{
	useIfNecessary();
	glUniform1f(location, val);
}
template<>
void Shader::setUniform(const int32_t location, const glm::vec1 &val)
{
	useIfNecessary();
	glUniform1f(location, val.x);
}
template<>
void Shader::setUniform(const int32_t location, const glm::vec2 &val)
{
	useIfNecessary();
	glUniform2f(location, val.x, val.y);
}
template<>
void Shader::setUniform(const int32_t location, const glm::vec3 &val)
{
	useIfNecessary();
	glUniform3f(location, val.x, val.y, val.z);
}
template<>
void Shader::setUniform(const int32_t location, const glm::vec4 &val)
{
	useIfNecessary();
	glUniform4f(location, val.x, val.y, val.z, val.w);
}
template<>
void Shader::setUniform(const int32_t location, const glm::mat4 &val)
{
	useIfNecessary();
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(val));
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
	for(auto const &it : textures)
	{
		glActiveTexture(GL_TEXTURE0 + it.first);
		it.second->use();
	}
}

void Shader::selectObjectUniforms(const uint32_t object)
{
	for(auto it = objectUniforms.begin(); it != objectUniforms.end(); it++)
	{
		(it->second)->selectObject(object);
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

	for(auto const &it : shaderUniforms)
	{
		delete it.second;
	}
	for(auto const &it : objectUniforms)
	{
		delete it.second;
	}
	release();
}
void Shader::release()
{
	lastUsed = 0;
	glUseProgram(0);
}

GLOBJ_LAST_USED(Shader)

IShaderUniform::IShaderUniform(Shader *const shader, const std::string &name) : shader(shader), name(name)
{
}


const std::string &IShaderUniform::getName() const
{
	return name;
}

template<typename Type>
ShaderUniform<Type>::ShaderUniform(Shader *const shader, const std::string &name, const Type &val) : IShaderUniform(shader, name), val(val)
{
}

template <typename Type>
void ShaderUniform<Type>::updateShader() const
{
	shader->setUniform<Type>(getName(), val);
}

template <typename Type>
void ShaderUniform<Type>::setValue(const Type &val)
{
	this->val = val;
	updateShader();
}

template <typename Type>
const Type &ShaderUniform<Type>::getValue() const
{
	return val;
}

template<typename Type>
ObjectShaderUniform<Type>::ObjectShaderUniform(Shader *const shader, const std::string &name, const Type &defVal) : IShaderUniform(shader, name), lastEditor(-2)
{
	specificValues[-2] = defVal;
}

template<typename Type>
ObjectShaderUniform<Type>::ObjectShaderUniform(Shader *const shader, const std::string &name, const Type &val, const int32_t object) : IShaderUniform(shader, name), lastEditor(object)
{
	specificValues[object] = val;
	updateShader();
}

template <typename Type>
void ObjectShaderUniform<Type>::updateShader() const
{
	shader->setUniform<Type>(getName(), specificValues.at(lastEditor));
}

template <typename Type>
void ObjectShaderUniform<Type>::setValue(const Type &val, const int32_t object)
{
	this->lastEditor = object;
	specificValues.insert_or_assign(lastEditor, val);
	updateShader();
}

template <typename Type>
void ObjectShaderUniform<Type>::setCurValue(const Type &val)
{
	lastEditor = -1;
	specificValues.insert_or_assign(lastEditor, val);
	updateShader();
}

template <typename Type>
void ObjectShaderUniform<Type>::selectObject(const int32_t object)
{
	if(lastEditor != object)
	{
		lastEditor = object;
		updateShader();
	}
}

template <typename Type>
const Type &ObjectShaderUniform<Type>::getValue(const int32_t object) const
{
	return specificValues.at(lastEditor);
}

template <typename Type>
const Type &ObjectShaderUniform<Type>::getCurValue() const
{
	return specificValues.at(lastEditor);
}

template <typename Type>
const int32_t ObjectShaderUniform<Type>::getLastEditor() const
{
	return lastEditor;
}

TEMPLATE_UNIFORM(bool)
TEMPLATE_UNIFORM(int)
TEMPLATE_UNIFORM(float)
TEMPLATE_UNIFORM(glm::vec1)
TEMPLATE_UNIFORM(glm::vec2)
TEMPLATE_UNIFORM(glm::vec3)
TEMPLATE_UNIFORM(glm::vec4)
TEMPLATE_UNIFORM(glm::mat4)