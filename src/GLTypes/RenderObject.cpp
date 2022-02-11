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




const glm::mat4 &RenderObject::getModel(bool forcedUpdate)
{
	if(!isModelChanged() && !forcedUpdate)
		return model;
	model = glm::translate(glm::mat4(1.0f), getPosition());
	model = glm::scale(model, getScale());
	const glm::vec3 &rotation = getRotation();
	model = model * glm::eulerAngleYZX(rotation.x, rotation.y, rotation.z);
	return model;
}

void RenderObject::updateMVP(FreeCamera &camera, bool forced)
{
	bool diffModel = forced || isModelChanged() || getObjectUniform<glm::mat4>("model.MVP")->getLastEditor() != rObjectID;
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
	if(camera.isVPUpdated() || forced)
	{
		shader->setShaderUniform("model.P", P);
		shader->setShaderUniform("model.VP", VP);
		shader->setShaderUniform("model.V", V);
		shader->setShaderUniform("camera.viewPos", camera.getCameraPos());
	}
	setChangedModel(false);
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