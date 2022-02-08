#include <GLTypes/RenderObject.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

RenderObjectData::RenderObjectData(BufferAccessType accessType, BufferCallType callType, DrawType drawType) : accessType(accessType), callType(callType), drawType(drawType)
{
}

RenderObject::RenderObject(uint32_t id, RenderObjectData objectParams, Shader *shader, VertexBuffer *vtxBuffer, VertexArray *vtxArray) : rObjetID(id), objectParams(objectParams), shader(shader), vtxBuffer(vtxBuffer), vtxArray(vtxArray), elementBuffer(nullptr)
{
}
RenderObject::RenderObject(uint32_t id, RenderObjectData objectParams, Shader *shader, VertexBuffer *vtxBuffer, VertexArray *vtxArray, ElementBuffer *elementBuffer) : rObjetID(id), objectParams(objectParams), shader(shader), vtxBuffer(vtxBuffer), vtxArray(vtxArray), elementBuffer(elementBuffer)
{
}
RenderObject::RenderObject(uint32_t id, const RenderObject &from) : shader(from.shader), vtxBuffer(from.vtxBuffer), vtxArray(from.vtxArray), elementBuffer(from.elementBuffer), rObjetID(id), uniforms(from.uniforms), objectParams(from.objectParams)
{
}

const glm::vec3 &RenderObject::getPosition() const
{
	return position;
}
const glm::vec3 &RenderObject::getRotation() const
{
	return rotation;
}
const glm::vec3 &RenderObject::getScale() const
{
	return scale;
}

template<typename Type>
const ShaderUniform<Type> *RenderObject::getShaderUniform(const std::string &name)
{
	return (ShaderUniform<Type>*)uniforms[name];
}

template<typename Type>
void RenderObject::setShaderUniformVal(const std::string &name, Type val)
{
	if(uniforms.find(name) == uniforms.end())
	{
		uniforms[name] = (IShaderUniform *)new ShaderUniform<Type>(name, val);
	}
	((ShaderUniform<Type>*)uniforms[name])->setValue(*shader, val);
}

void RenderObject::setPosition(const glm::vec3 &position)
{
	this->position = position;
	changedModel = true;
}
void RenderObject::setRotation(const glm::vec3 &rotation)
{
	this->rotation = rotation;
	changedModel = true;
}
void RenderObject::setScale(const glm::vec3 &scale)
{
	this->scale = scale;
	changedModel = true;
}

const glm::mat4 &RenderObject::getModel(bool forcedUpdate)
{
	if(!changedModel && !forcedUpdate)
		return model;
	model = glm::translate(glm::mat4(1.0f), position);
	model = glm::scale(model, scale);
	model = model * glm::eulerAngleYZX(rotation.x, rotation.y, rotation.z);
	return model;
}

void RenderObject::updateMVP(const glm::mat4 &VP, bool VPUpdated)
{
	getModel();
	if(!(VPUpdated || changedModel || model != getShaderUniform<glm::mat4>("MVP")->getValue()))
		return;
	glm::mat4 MVP = VP * model;
	setShaderUniformVal("MVP", MVP);
	changedModel = false;
}

void RenderObject::draw(const glm::mat4 &VP, bool VPUpdated)
{
	updateMVP(VP, VPUpdated);
	vtxArray->useIfNecessary();
	if(elementBuffer != nullptr)
	{
		elementBuffer->drawElements(objectParams.drawType);
		return;
	}
	vtxArray->drawArray(objectParams.drawType);
}