#pragma once

#include <glad/glad.h>
#include <iostream>
#include <unordered_map>
#include <GLTypes/GLObject.h>
#include <GLTypes/Texture.h>
#include <glm/glm.hpp>

class Shader;


class IShaderUniform
{
private:
	const std::string name;
protected:
	Shader *const shader;
public:
	IShaderUniform(Shader *const shader, const std::string &name);
	virtual void updateShader() const = 0;
	const std::string &getName() const;
	virtual void selectObject(const int32_t object) = 0;
};

template<typename Type>
class ShaderUniform : public IShaderUniform
{
private:
	Type val;
public:
	ShaderUniform(Shader *const shader, const std::string &name, const Type &val);
	void setValue(const Type &newVal);
	void updateShader() const override;
	void selectObject(const int32_t object) override
	{
	}
	const Type &getValue() const;
};

template<typename Type>
class ObjectShaderUniform : public IShaderUniform
{
private:
	int32_t lastEditor;
	std::unordered_map<int32_t, Type> specificValues;
public:
	ObjectShaderUniform(Shader *const shader, const std::string &name, const Type &defVal);
	ObjectShaderUniform(Shader *const shader, const std::string &name, const Type &val, const int32_t object);
	void setValue(const Type &newVal, const int32_t object);
	/*
		Sets cur value without changing object specific values
		Use only if setting values without object dependency
	*/
	void setCurValue(const Type &newVal);
	void updateShader() const override;
	void selectObject(const int32_t object) override;
	const Type &getValue(const int32_t object) const;
	const Type &getCurValue() const;
	const int32_t getLastEditor() const;
};


class Shader : public GLObject
{
private:
	std::unordered_map<uint32_t, Texture2D *> textures;
	std::unordered_map<std::string, IShaderUniform *> shaderUniforms, objectUniforms;
	std::unordered_map<std::string, int32_t> UniformLocations;
public:
	Shader(const char *vtxPath, const char *fragPath);

	int32_t getUniformLocation(const std::string &name);

	template <typename T>
	void setUniform(const int32_t location, const T &val);
	template <typename T>
	void setUniform(const std::string &name, const T &val)
	{
		int32_t location = getUniformLocation(name);
		if(location != -1)
			setUniform(location, val);
	}
	uint32_t addTexture(const std::string &name, Texture2D *texture, int32_t id = -1);
	Texture2D *getTexture(const uint32_t textureId);


	template<typename Type>
	const ShaderUniform<Type> *getShaderUniform(const std::string &name)
	{
		return (ShaderUniform<Type>*)shaderUniforms[name];
	}

	template<typename Type>
	void setShaderUniform(const std::string &name, const Type &val)
	{
		if(shaderUniforms.find(name) == shaderUniforms.end())
		{
			shaderUniforms[name] = (IShaderUniform *)new ShaderUniform<Type>(this, name, val);
		}
		((ShaderUniform<Type>*)shaderUniforms[name])->setValue(val);
	}

	void selectObjectUniforms(const uint32_t object);

	template<typename Type>
	const Type &getCurObjectUniformVal(const std::string &name)
	{
		return ((ObjectShaderUniform<Type>*)objectUniforms[name])->getCurValue();
	}

	template<typename Type>
	const ObjectShaderUniform<Type> *getObjectUniform(const std::string &name)
	{
		return (ObjectShaderUniform<Type>*)objectUniforms[name];
	}

	template<typename Type>
	const ObjectShaderUniform<Type> *getObjectUniformVal(const std::string &name, const int32_t object)
	{
		return ((ObjectShaderUniform<Type>*)objectUniforms[name])->getValue(object);
	}

	template<typename Type>
	void setObjectUniform(const std::string &name, const Type &val, const int32_t object)
	{
		if(objectUniforms.find(name) == objectUniforms.end())
		{
			objectUniforms[name] = (IShaderUniform *)new ObjectShaderUniform<Type>(this, name, val, object);
		}
		else
		{
			((ObjectShaderUniform<Type>*)objectUniforms[name])->setValue(val, object);
		}
	}

	void useTextures();
	GLOBJ_OVERRIDE(Shader)
};

#define TEMPLATE_UNIFORM(x) template ShaderUniform<x>; template ObjectShaderUniform<x>;


