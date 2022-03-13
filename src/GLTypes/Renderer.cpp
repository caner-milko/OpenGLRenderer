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
	addRenderObject(renderObject);
	return renderObject;
}

SimpleRenderObject *Renderer::addSimpleObject(RenderObjectData data, Shader *shader, const std::vector<float> &vertices, const std::vector<uint32_t> &attributeSizes, const std::vector<uint32_t> &indices)
{
	uint32_t id = itemCount++;
	SimpleRenderObject *renderObject = new SimpleRenderObject(id, data, shader, vertices, attributeSizes, indices);
	addRenderObject(renderObject);
	return renderObject;
}

Mesh *Renderer::addAssimpMesh(RenderObjectData data, Shader *shader, AssimpMesh &assimpMesh)
{
	uint32_t id = itemCount++;
	Mesh *mesh = new Mesh(id, data, shader, assimpMesh.vertices, assimpMesh.indices);
	addRenderObject(mesh);
	for(auto &texture : assimpMesh.textures)
	{
		shader->setUniformValObj<Texture2D *>("material." + texture.type, texture.texture, id);
	}
	return mesh;
}

std::vector<Mesh *> Renderer::addModel(RenderObjectData data, Shader *shader, const char *path)
{
	AssimpModel model(path);
	std::vector<Mesh *> meshes;
	for(auto &mesh : model.meshes)
	{
		meshes.push_back(addAssimpMesh(data, shader, mesh));
	}

	return meshes;
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

void Renderer::addRenderObject(RenderObject *renderObject)
{
	renderObject->initObject();
	vertexBuffers.push_back(renderObject->getVertexBuffer());
	vertexArrays.push_back(renderObject->getVertexArray());
	if(renderObject->hasElementBuffer())
		elementBuffers.push_back(renderObject->getElementBuffer());

	renderObjects.push_back(renderObject);
}