#pragma once

#include <iostream>
#include <glm/glm.hpp>

#include <GLTypes/VertexArray.h>
#include <GLTypes/VertexBuffer.h>
#include <GLTypes/ElementBuffer.h>
#include <GLTypes/Shader.h>
#include <GLTypes/Camera.h>

#include <unordered_map>
#include <string>

struct RenderObjectData
{
	const BufferAccessType accessType;
	const BufferCallType callType;

	DrawType drawType;

	RenderObjectData(BufferAccessType accessType = BufferAccessType::STATIC, BufferCallType callType = BufferCallType::DRAW, DrawType drawType = DrawType::TRIANGLES);
};

class RenderObject
{

private:
	VertexBuffer *vtxBuffer;
	VertexArray *vtxArray;
	ElementBuffer *elementBuffer;
	Shader *shader;

	bool changedModel = true;
	glm::mat4 model = glm::mat4(1.0f);


	glm::vec3 position = glm::vec3(0.0f), rotation = glm::vec3(0.0f), scale = glm::vec3(1.0f);

	std::unordered_map<std::string, IShaderUniform *> uniforms;

public:
	RenderObjectData objectParams;
	const uint32_t rObjetID;
	RenderObject(uint32_t id, RenderObjectData objectParams, Shader *shader, VertexBuffer *vtxBuffer, VertexArray *vtxArray);
	RenderObject(uint32_t id, RenderObjectData objectParams, Shader *shader, VertexBuffer *vtxBuffer, VertexArray *vtxArray, ElementBuffer *elementBuffer);
	RenderObject(uint32_t id, const RenderObject &from);

	const glm::vec3 &getPosition() const;
	const glm::vec3 &getRotation() const;
	const glm::vec3 &getScale() const;

	template<typename Type>
	const ShaderUniform<Type> *getShaderUniform(const std::string &name)
	{
		return (ShaderUniform<Type>*)uniforms[name];
	}

	template<typename Type>
	void setShaderUniformVal(const std::string &name, const Type &val)
	{
		if(uniforms.find(name) == uniforms.end())
		{
			uniforms[name] = (IShaderUniform *)new ShaderUniform<Type>(name, val);
		}
		((ShaderUniform<Type>*)uniforms[name])->setValue(*shader, val);
	}

	void setPosition(const glm::vec3 &position);
	void setRotation(const glm::vec3 &rotation);
	void setScale(const glm::vec3 &scale);


	const glm::mat4 &getModel(bool forcedUpdate = false);
	void updateMVP(FreeCamera &camera);

	void draw(FreeCamera &camera);
};