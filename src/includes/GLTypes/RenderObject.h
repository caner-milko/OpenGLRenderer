#pragma once

#include <iostream>
#include <glm/glm.hpp>

#include <GLTypes/Transform.h>
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

class RenderObject : public Transform
{

private:

	bool first = true;
	glm::mat4 model = glm::mat4(1.0f);

public:
	VertexBuffer *const vtxBuffer;
	VertexArray *const vtxArray;
	ElementBuffer *const elementBuffer;
	Shader *const shader;
	RenderObjectData objectParams;
	const uint32_t rObjectID;
	RenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader, VertexBuffer *const vtxBuffer, VertexArray *const vtxArray);
	RenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader, VertexBuffer *const vtxBuffer, VertexArray *const vtxArray, ElementBuffer *const elementBuffer);
	RenderObject(uint32_t id, const RenderObject &from);

	bool hasElementBuffer() const;

	template<typename Type>
	const ObjectShaderUniform<Type> *getObjectUniform(const std::string &name)
	{
		return shader->getObjectUniform<Type>(name);
	}

	template<typename Type>
	const ObjectShaderUniform<Type> *getObjectUniformVal(const std::string &name)
	{
		return shader->getObjectUniformVal<Type>(name, rObjectID);
	}

	template<typename Type>
	void setObjectUniformVal(const std::string &name, const Type &val)
	{
		shader->setObjectUniform<Type>(name, val, rObjectID);
	}

	const glm::mat4 &getModel(bool forcedUpdate = false);
	void updateMVP(FreeCamera &camera, bool forced = false);

	void draw(FreeCamera &camera, bool forced = false);
};