#include <GLTypes/Mesh.h>

Mesh::Mesh(uint32_t id, RenderObjectData objectParams, Shader *const shader, const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) : RenderObject(id, objectParams, shader), vertices(vertices), indices(indices)
{

}


void Mesh::initObject()
{
	vtxArray = new VertexArray();
	std::vector<uint32_t> attributesSizes = {3,3,2};
	std::vector<float> verticesFloat;
	verticesFloat.reserve(vertices.size() * 8);
	for(auto &vertex : vertices)
		verticesFloat.insert(verticesFloat.end(), (float *)&vertex, (float *)(&vertex + 1));
	vtxBuffer = new VertexBuffer(*vtxArray, verticesFloat, attributesSizes, BufferAccessType::STATIC, BufferCallType::DRAW);
	elementBuffer = new ElementBuffer(indices, BufferAccessType::STATIC, BufferCallType::DRAW);
}