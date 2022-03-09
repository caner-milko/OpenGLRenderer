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
public:
	const std::string name;
	Shader *const shader;
	IShaderUniform(Shader *const shader, const std::string &name);
	virtual void updateShader() const = 0;
	virtual void selectObject(const int32_t object) = 0;
};


template<typename Type>
class ObjectShaderUniform : public IShaderUniform
{
private:
	int32_t lastEditor;
	Type defVal;
	std::unordered_map<int32_t, Type> specificValues;
public:
	ObjectShaderUniform(Shader *const shader, const std::string &name, const Type &defVal);
	ObjectShaderUniform(Shader *const shader, const std::string &name, const Type &val, const int32_t object);
	void setValue(const Type &newVal, const int32_t object);
	void updateShader() const override;
	void selectObject(const int32_t object) override;
	const Type &getValue(const int32_t object) const;
	const Type &getCurValue() const;
	const int32_t getLastEditor() const;
};


template<>
class ObjectShaderUniform<Texture2D *> : public IShaderUniform
{
private:
	int32_t lastEditor;
	Texture2D *defVal;
	std::unordered_map<int32_t, Texture2D *> specificValues;
public:
	ObjectShaderUniform(Shader *const shader, const std::string &name, Texture2D *const defVal);
	ObjectShaderUniform(Shader *const shader, const std::string &name, Texture2D *const val, const int32_t object);

	void setValue(Texture2D *const &newVal, const int32_t object);
	void selectObject(const int32_t object) override;
	Texture2D *const &getValue(const int32_t object) const;
	Texture2D *const &getCurValue() const;
	const int32_t getLastEditor() const;

	const uint32_t texLoc;
	uint32_t setupPosition();
	void updateShader() const override;
};

class Shader : public GLObject
{
private:
	std::unordered_map<std::string, IShaderUniform *> objectUniforms;
	std::unordered_map<std::string, int32_t> UniformLocations;
public:
	uint32_t addedTextureCount = 0;
	const uint32_t textureCount;
	Shader(const char *vtxPath, const char *fragPath, uint32_t textureCount = 0); // TODO combine vtx frag shaders to a single file, read textureCount from there

	int32_t getUniformLocation(const std::string &name);

	void initTextures();

	void selectObjectUniforms(const uint32_t object);

	template <typename T>
	void setUniform(const int32_t location, const T &val);
	template <typename T>
	void setUniform(const std::string &name, const T &val)
	{
		int32_t location = getUniformLocation(name);
		if(location != -1)
			setUniform(location, val);
	}



	template<typename Type>
	const Type &getCurObjectUniformVal(const std::string &name)
	{
		return ((ObjectShaderUniform<Type>*)objectUniforms[name])->getCurValue();
	}

	template<typename Type>
	const ObjectShaderUniform<Type> *getUniform(const std::string &name)
	{
		return (ObjectShaderUniform<Type>*)objectUniforms[name];
	}

	template<typename Type>
	const Type &getUniformValObj(const std::string &name, const int32_t object)
	{
		return ((ObjectShaderUniform<Type>*)objectUniforms[name])->getValue(object);
	}

	template<typename Type>
	void setUniformValObj(const std::string &name, const Type &val, const int32_t object)
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

	template<typename Type>
	const Type &getUniformValDef(const std::string &name)
	{
		return ((ObjectShaderUniform<Type>*)objectUniforms[name])->getValue(-1);
	}

	template<typename Type>
	void setUniformValDef(const std::string &name, const Type &defVal)
	{
		if(objectUniforms.find(name) == objectUniforms.end())
		{
			objectUniforms[name] = (IShaderUniform *)new ObjectShaderUniform<Type>(this, name, defVal);
		}
		else
		{
			((ObjectShaderUniform<Type>*)objectUniforms[name])->setValue(defVal, -1);
		}
	}



	GLOBJ_OVERRIDE(Shader)
};

#define TEMPLATE_UNIFORM(x) /*template ShaderUniform<x>;*/ template ObjectShaderUniform<x>;


