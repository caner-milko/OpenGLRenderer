#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>

#include <GLTypes/RenderObject.h>
#include <GLTypes/Camera.h>
#include <GLTypes/ElementBuffer.h>
#include <GLTypes/VertexBuffer.h>
#include <GLTypes/VertexArray.h>


struct RendererData
{
	bool clearColor = true, depthTest = true, clearDepth = true;
	float resetCol[4] = {0.2f, 0.3f, 0.8f, 1.0f};
	uint32_t getClearMask();
};

class Renderer
{
	uint32_t itemCount = 0;



	std::unordered_map<uint32_t, VertexBuffer *> vtxBuffers;
	std::unordered_map<uint32_t, VertexArray *> vtxArrays;
	std::unordered_map<uint32_t, ElementBuffer *> elementBuffers;
	std::vector<RenderObject *> renderObjects;
	RendererData data;
public:
	FreeCamera *camera;
	static Renderer *renderer;

	Renderer(RendererData data);

	RenderObject *addOddObject(RenderObjectData data, Shader *shader, const std::vector<float> &vertices, const std::vector<uint32_t> &attributeSizes);

	RenderObject *addMesh(RenderObjectData data, Shader *shader, const std::vector<float> &vertices, const std::vector<uint32_t> &attributeSizes, const std::vector<uint32_t> &indices);

	RenderObject *cloneObject(uint32_t id);

	void draw();

	void free();

	~Renderer();
};