#include <GLTypes/RenderObject.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

RenderObjectData::RenderObjectData(BufferAccessType accessType, BufferCallType callType, DrawType drawType) : accessType(accessType), callType(callType), drawType(drawType)
{
}

RenderObject::RenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader) : rObjectID(id), objectParams(objectParams), shader(shader)
{
}

RenderObject::RenderObject(uint32_t id, const RenderObject &from) : rObjectID(id), objectParams(from.objectParams), shader(from.shader), vtxBuffer(from.vtxBuffer), vtxArray(from.vtxArray), elementBuffer(from.elementBuffer)
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
	if(elementBuffer != nullptr)
	{
		vtxArray->drawElements(*elementBuffer, objectParams.drawType);
		return;
	}
	vtxArray->drawArray(*vtxBuffer, objectParams.drawType);
}

const VertexBuffer *RenderObject::getVertexBuffer()
{
	return vtxBuffer;
}
const VertexArray *RenderObject::getVertexArray()
{
	return vtxArray;
}
const ElementBuffer *RenderObject::getElementBuffer()
{
	return elementBuffer;
}

SimpleRenderObject::SimpleRenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader, std::vector<float> vertices, std::vector<uint32_t> attributeSizes) : RenderObject(id, objectParams, shader), vertices(vertices), attributeSizes(attributeSizes)
{
}
SimpleRenderObject::SimpleRenderObject(uint32_t id, RenderObjectData objectParams, Shader *const shader, std::vector<float> vertices, std::vector<uint32_t> attributeSizes, std::vector<uint32_t> indices) : RenderObject(id, objectParams, shader), vertices(vertices), attributeSizes(attributeSizes), indices(indices)
{
}
SimpleRenderObject::SimpleRenderObject(uint32_t id, const SimpleRenderObject &from) : RenderObject(id, from), vertices(from.vertices), attributeSizes(from.attributeSizes), indices(from.indices)
{
}
void SimpleRenderObject::initObject()
{
	vtxArray = new VertexArray();
	vtxBuffer = new VertexBuffer(*vtxArray, vertices, attributeSizes, BufferAccessType::STATIC, BufferCallType::DRAW);
	if(indices.size() != 0)
		elementBuffer = new ElementBuffer(indices, BufferAccessType::STATIC, BufferCallType::DRAW);
}

const std::vector<float> &SimpleRenderObject::getVertices()
{
	return vertices;
}
const std::vector<uint32_t> &SimpleRenderObject::getAttributeSizes()
{
	return attributeSizes;
}
const std::vector<uint32_t> &SimpleRenderObject::getIndices()
{
	return indices;
}