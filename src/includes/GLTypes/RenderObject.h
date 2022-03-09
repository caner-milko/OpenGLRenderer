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

protected:
	VertexBuffer *vtxBuffer = nullptr;
	VertexArray *vtxArray = nullptr;
	ElementBuffer *elementBuffer = nullptr;
public:
	Shader *const shader;
	RenderObjectData objectParams;
	const uint32_t rObjectID;
	RenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader);
	RenderObject(uint32_t id, const RenderObject &from);

	virtual void initObject() = 0;

	bool hasElementBuffer() const;

	template<typename Type>
	const ObjectShaderUniform<Type> *getObjectUniform(const std::string &name)
	{
		return shader->getUniform<Type>(name);
	}

	template<typename Type>
	const Type &getObjectUniformVal(const std::string &name)
	{
		return shader->getUniformValObj<Type>(name, rObjectID);
	}

	template<typename Type>
	void setObjectUniformVal(const std::string &name, const Type &val)
	{
		shader->setUniformValObj<Type>(name, val, rObjectID);
	}

	const glm::mat4 &getModel(bool forcedUpdate = false);
	void updateMVP(FreeCamera &camera, bool forced = false);

	void draw(FreeCamera &camera, bool forced = false);

	const VertexBuffer *getVertexBuffer();
	const VertexArray *getVertexArray();
	const ElementBuffer *getElementBuffer();
};

class SimpleRenderObject : public RenderObject
{
private:
	std::vector<float> vertices;
	std::vector<uint32_t> attributeSizes, indices;
public:
	SimpleRenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader, std::vector<float> vertices, std::vector<uint32_t> attributeSizes);
	SimpleRenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader, std::vector<float> vertices, std::vector<uint32_t> attributeSizes, std::vector<uint32_t> indices);
	SimpleRenderObject(uint32_t id, const SimpleRenderObject &from);
	virtual void initObject() override;
	const std::vector<float> &getVertices();
	const std::vector<uint32_t> &getAttributeSizes();
	const std::vector<uint32_t> &getIndices();
};