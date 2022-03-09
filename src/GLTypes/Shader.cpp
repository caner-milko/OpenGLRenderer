#pragma once
#include <GLTypes/Shader.h>
#include <Utils/FileUtils.h>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char *vtxPath, const char *fragPath, uint32_t textureCount) : textureCount(textureCount)
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
	initTextures();
}

void Shader::initTextures() // TODO implement
{

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

void Shader::selectObjectUniforms(const uint32_t object)
{
	for(auto &it : objectUniforms)
	{
		it.second->selectObject(object);
	}
}

void Shader::use()
{
	glUseProgram(id);
	lastUsed = id;
}
void Shader::free()
{
	glDeleteProgram(id);
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

template<typename Type>
ObjectShaderUniform<Type>::ObjectShaderUniform(Shader *const shader, const std::string &name, const Type &defVal) : IShaderUniform(shader, name), lastEditor(-1), defVal(defVal)
{
	updateShader();
}

template<typename Type>
ObjectShaderUniform<Type>::ObjectShaderUniform(Shader *const shader, const std::string &name, const Type &val, const int32_t object) : IShaderUniform(shader, name), lastEditor(object), defVal(val)
{
	specificValues[object] = val;
	updateShader();
}

template <typename Type>
void ObjectShaderUniform<Type>::updateShader() const
{
	if(lastEditor == -1)
	{
		shader->setUniform<Type>(name, defVal);
	}
	else
	{
		shader->setUniform<Type>(name, specificValues.at(lastEditor));
	}
}

template <typename Type>
void ObjectShaderUniform<Type>::setValue(const Type &val, const int32_t object)
{
	if(object == -1)
	{
		lastEditor = -1;
		defVal = val;
	}
	else
	{
		lastEditor = object;
		specificValues.insert_or_assign(lastEditor, val);
	}
	updateShader();
}


template <typename Type>
void ObjectShaderUniform<Type>::selectObject(const int32_t object)
{
	int32_t selected = object;
	if(specificValues.find(object) == specificValues.end())
	{
		selected = -1;
	}
	if(lastEditor != selected)
	{
		lastEditor = selected;
		updateShader();
	}
}

template <typename Type>
const Type &ObjectShaderUniform<Type>::getValue(const int32_t object) const
{
	return object == -1 ? defVal : specificValues.at(object);
}

template <typename Type>
const Type &ObjectShaderUniform<Type>::getCurValue() const
{
	return lastEditor == -1 ? defVal : specificValues.at(lastEditor);
}

template <typename Type>
const int32_t ObjectShaderUniform<Type>::getLastEditor() const
{
	return lastEditor;
}

ObjectShaderUniform<Texture2D *>::ObjectShaderUniform(Shader *const shader, const std::string &name, Texture2D *const defVal) : IShaderUniform(shader, name), lastEditor(-1), defVal(defVal), texLoc(setupPosition())
{
}

ObjectShaderUniform<Texture2D *>::ObjectShaderUniform(Shader *const shader, const std::string &name, Texture2D *const val, const int32_t object) : IShaderUniform(shader, name), lastEditor(object), defVal(val), texLoc(setupPosition())
{
}


uint32_t ObjectShaderUniform<Texture2D *>::setupPosition()
{
	int32_t pos = shader->addedTextureCount++;
	glActiveTexture(GL_TEXTURE0 + pos);
	shader->setUniform<int32_t>(name, pos);
	getCurValue()->use();
	glActiveTexture(GL_TEXTURE0);
	return pos;
}

void ObjectShaderUniform<Texture2D *>::updateShader() const
{
	glActiveTexture(GL_TEXTURE0 + texLoc);
	getCurValue()->use();
}

void ObjectShaderUniform<Texture2D *>::setValue(Texture2D *const &val, const int32_t object)
{
	if(object == -1)
	{
		this->lastEditor = -1;
		defVal = val;
	}
	else
	{
		this->lastEditor = object;
		specificValues.insert_or_assign(lastEditor, val);
	}
	updateShader();
}


void ObjectShaderUniform<Texture2D *>::selectObject(const int32_t object)
{
	int32_t selected = object;
	if(specificValues.find(object) == specificValues.end())
	{
		selected = -1;
	}
	if(lastEditor != selected)
	{
		lastEditor = selected;
		updateShader();
	}
}

Texture2D *const &ObjectShaderUniform<Texture2D *>::getValue(const int32_t object) const
{
	return object == -1 ? defVal : specificValues.at(object);
}

Texture2D *const &ObjectShaderUniform<Texture2D *>::getCurValue() const
{
	return lastEditor == -1 ? defVal : specificValues.at(lastEditor);
}

const int32_t ObjectShaderUniform<Texture2D *>::getLastEditor() const
{
	return lastEditor;
}

TEMPLATE_UNIFORM(bool)
TEMPLATE_UNIFORM(int32_t)
TEMPLATE_UNIFORM(float)
TEMPLATE_UNIFORM(glm::vec1)
TEMPLATE_UNIFORM(glm::vec2)
TEMPLATE_UNIFORM(glm::vec3)
TEMPLATE_UNIFORM(glm::vec4)
TEMPLATE_UNIFORM(glm::mat4)