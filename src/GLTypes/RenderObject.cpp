#include <GLTypes/RenderObject.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

RenderObjectData::RenderObjectData(BufferAccessType accessType, BufferCallType callType, DrawType drawType) : accessType(accessType), callType(callType), drawType(drawType)
{
}

RenderObject::RenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader, VertexBuffer *const vtxBuffer, VertexArray *const vtxArray) : rObjectID(id), objectParams(objectParams), shader(shader), vtxBuffer(vtxBuffer), vtxArray(vtxArray), elementBuffer(nullptr)
{
}
RenderObject::RenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader, VertexBuffer *vtxBuffer, VertexArray *const vtxArray, ElementBuffer *const elementBuffer) : rObjectID(id), objectParams(objectParams), shader(shader), vtxBuffer(vtxBuffer), vtxArray(vtxArray), elementBuffer(elementBuffer)
{
}
RenderObject::RenderObject(uint32_t id, const RenderObject &from) : shader(from.shader), vtxBuffer(from.vtxBuffer), vtxArray(from.vtxArray), elementBuffer(from.elementBuffer), rObjectID(id), objectParams(from.objectParams)
{
}

bool RenderObject::hasElementBuffer() const
{
	return elementBuffer == nullptr;
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

void RenderObject::updateMVP(FreeCamera &camera, bool forced)
{
	bool diffModel = forced || changedModel || getObjectUniform<glm::mat4>("model.MVP")->getLastEditor() != rObjectID;
	if(!(camera.isVPUpdated() || diffModel || forced))
		return;
	getModel();
	const glm::mat4 &VP = camera.getVPMatrix();
	const glm::mat4 &V = camera.getViewMatrix();
	const glm::mat4 &P = camera.getProjectionMatrix();
	glm::mat4 MVP = VP * model;
	glm::mat4 MV = V * model;
	if(diffModel)
	{
		setObjectUniformVal("model.M", model);
		const glm::mat4 inverseModel = glm::inverse(model);
		setObjectUniformVal("model.IM", inverseModel);
		setObjectUniformVal("model.TIM", glm::transpose(inverseModel));
	}

	setObjectUniformVal("model.MVP", MVP);
	setObjectUniformVal("model.MV", MV);
	if(forced || camera.isVPUpdated())
	{
		shader->setShaderUniform("model.P", P);
		shader->setShaderUniform("model.VP", VP);
		shader->setShaderUniform("model.V", V);
		shader->setShaderUniform("camera.viewPos", camera.getCameraPos());
	}
	changedModel = false;
}

void RenderObject::draw(FreeCamera &camera, bool forced)
{
	shader->use();
	updateMVP(camera, forced || first);
	first = false;
	shader->selectObjectUniforms(rObjectID);
	vtxArray->useIfNecessary();
	if(elementBuffer != nullptr)
	{
		elementBuffer->drawElements(objectParams.drawType);
		return;
	}
	vtxArray->drawArray(objectParams.drawType);
}