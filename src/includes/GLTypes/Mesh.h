#pragma once
#include <GLTypes/Vertex.h>
#include <GLTypes/RenderObject.h>
class Mesh : public RenderObject
{
private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
public:
	Mesh(uint32_t id, RenderObjectData objectParams, Shader *const shader, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices);

	virtual void initObject() override;

};