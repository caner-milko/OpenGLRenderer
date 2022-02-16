#include <GLTypes/Renderer.h>

uint32_t RendererData::getClearMask()
{
	return (clearColor * GL_COLOR_BUFFER_BIT) | (clearDepth * GL_DEPTH_BUFFER_BIT);
}

Renderer *Renderer::renderer;
Renderer::Renderer(RendererData data) : camera(new FreeCamera(glm::vec3(0.0f))), data(data), lightManager(new LightManager())
{
	renderer = this;
	if(data.depthTest)
	{
		glEnable(GL_DEPTH_TEST);
	}
}

SimpleRenderObject *Renderer::addOddObject(RenderObjectData data, Shader *shader, const std::vector<float> &vertices, const std::vector<uint32_t> &attributeSizes)
{
	uint32_t id = itemCount++;
	SimpleRenderObject *renderObject = new SimpleRenderObject(id, data, shader, vertices, attributeSizes);
	renderObject->initObject();

	vertexBuffers.push_back(renderObject->getVertexBuffer());
	vertexArrays.push_back(renderObject->getVertexArray());
	elementBuffers.push_back(renderObject->getElementBuffer());

	renderObjects.push_back(renderObject);
	return renderObject;
}

SimpleRenderObject *Renderer::addSimpleObject(RenderObjectData data, Shader *shader, const std::vector<float> &vertices, const std::vector<uint32_t> &attributeSizes, const std::vector<uint32_t> &indices)
{
	uint32_t id = itemCount++;
	SimpleRenderObject *renderObject = new SimpleRenderObject(id, data, shader, vertices, attributeSizes, indices);
	renderObject->initObject();
	vertexBuffers.push_back(renderObject->getVertexBuffer());
	vertexArrays.push_back(renderObject->getVertexArray());
	elementBuffers.push_back(renderObject->getElementBuffer());

	renderObjects.push_back(renderObject);
	return renderObject;
}

void Renderer::draw()
{
	glClearColor(data.resetCol[0], data.resetCol[1], data.resetCol[2], data.resetCol[3]);
	glClear(data.getClearMask());

	camera->updateMatrices();

	lightManager->updateLights();

	for(auto const &ro : renderObjects)
	{
		ro->draw(*camera);
	}

	camera->clearFlags();
}

Renderer::~Renderer()
{
	std::cout << "Renderer deleted." << std::endl;
	delete lightManager;
	delete camera;
	for(auto const &ro : renderObjects)
	{
		delete ro;
	}
	for(auto &it : vertexBuffers)
	{
		delete it;
	}
	for(auto &it : vertexArrays)
	{
		delete it;
	}
	for(auto &it : elementBuffers)
	{
		delete it;
	}

}