#include <GLTypes/Renderer.h>

uint32_t RendererData::getClearMask()
{
	return (clearColor * GL_COLOR_BUFFER_BIT) | (clearDepth * GL_DEPTH_BUFFER_BIT);
}

Renderer *Renderer::renderer;
Renderer::Renderer(RendererData data) : camera(new FreeCamera(glm::vec3(0.0f))), data(data)
{
	renderer = this;
	if(data.depthTest)
	{
		glEnable(GL_DEPTH_TEST);
	}
}

RenderObject *Renderer::addOddObject(RenderObjectData data, Shader *shader, const std::vector<float> &vertices, const std::vector<uint32_t> &attributeSizes)
{
	uint32_t id = itemCount++;
	VertexBuffer *vtxBuffer = new VertexBuffer(attributeSizes);
	VertexArray *vtxArray = new VertexArray(*vtxBuffer);

	vtxBuffers[id] = vtxBuffer;
	vtxArrays[id] = vtxArray;

	vtxArray->useIfNecessary();
	vtxBuffer->addVertex(vertices);

	vtxBuffer->loadVertices(data.accessType, data.callType);

	vtxBuffer->release();
	vtxArray->release();
	RenderObject *renderObject = new RenderObject(id, data, shader, vtxBuffer, vtxArray);

	renderObjects.push_back(renderObject);
	return renderObject;
}

RenderObject *Renderer::addMesh(RenderObjectData data, Shader *shader, const std::vector<float> &vertices, const std::vector<uint32_t> &attributeSizes, const std::vector<uint32_t> &indices)
{
	uint32_t id = itemCount++;
	VertexBuffer *vtxBuffer = new VertexBuffer(attributeSizes);
	VertexArray *vtxArray = new VertexArray(*vtxBuffer);
	ElementBuffer *elementBuffer = new ElementBuffer();

	vtxBuffers[id] = vtxBuffer;
	vtxArrays[id] = vtxArray;
	elementBuffers[id] = elementBuffer;

	vtxArray->useIfNecessary();
	vtxBuffer->addVertex(vertices);

	elementBuffer->addTriangles(indices);
	vtxBuffer->loadVertices(data.accessType, data.callType);
	elementBuffer->loadElements(data.accessType, data.callType);

	vtxBuffer->release();
	vtxArray->release();
	elementBuffer->release();
	RenderObject *renderObject = new RenderObject(id, data, shader, vtxBuffer, vtxArray, elementBuffer);

	renderObjects.push_back(renderObject);
	return renderObject;
}

RenderObject *Renderer::cloneObject(uint32_t clone)
{
	uint32_t id = itemCount++;
	RenderObject *clonedObject = new RenderObject(id, *renderObjects[clone]);
	renderObjects.push_back(clonedObject);

	return clonedObject;
}


void Renderer::draw()
{
	glClearColor(data.resetCol[0], data.resetCol[1], data.resetCol[2], data.resetCol[3]);
	glClear(data.getClearMask());

	camera->updateMatrices();

	for(auto const &ro : renderObjects)
	{
		ro->draw(*camera);
	}

	camera->clearFlags();
}

Renderer::~Renderer()
{
	std::cout << "Renderer delete" << std::endl;
	delete camera;
	for(auto const &ro : renderObjects)
	{
		delete ro;
	}
	for(auto const &it : vtxBuffers)
	{
		delete it.second;
	}
	for(auto const &it : vtxArrays)
	{
		delete it.second;
	}
	for(auto const &it : elementBuffers)
	{
		delete it.second;
	}

}