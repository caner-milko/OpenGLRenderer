#pragma once

#include <glad/glad.h>
#include <iostream>
#include <unordered_map>
#include <GLTypes/GLObject.h>
#include <GLTypes/Texture.h>
#include <glm/glm.hpp>

class Shader : public GLObject
{
private:
	std::unordered_map<uint32_t, Texture2D *> textures;
public:
	std::unordered_map<std::string, int32_t> UniformLocations;
	Shader(const char *vtxPath, const char *fragPath);
	int32_t getUniformLocation(const std::string &name);
	template <typename T>
	void setUniform(const std::string &name, const T &val);

	uint32_t addTexture(const std::string &name, Texture2D *texture, int32_t id = -1);
	Texture2D *getTexture(const uint32_t textureId);

	void useTextures();
	GLOBJ_OVERRIDE(Shader)
};

class IShaderUniform
{
private:
	const std::string name;
public:
	IShaderUniform(const std::string &name);
	virtual void updateShader(Shader &shader) const = 0;
	const std::string &getName() const;
};

template<typename Type>
class ShaderUniform : public IShaderUniform
{
private:
	Type val;
public:
	ShaderUniform(const std::string &name, const Type &val);
	void setValue(Shader &shader, const Type &newVal);
	void updateShader(Shader &shader) const override;
	const Type &getValue() const;
};