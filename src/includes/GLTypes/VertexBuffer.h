#pragma once
#include <glad/glad.h>
#include <vector>
#include <GLTypes/GLObject.h>

enum class BufferAccessType
{
	STREAM = 0x0, STATIC = 0x4, DYNAMIC = 0x8

};

enum class BufferCallType
{
	DRAW = 0, READ = 1, COPY = 2

};

class VertexBuffer : public GLObject
{
public:
	std::vector<float> vertices;
	std::vector<uint32_t> attributeSizes;

	VertexBuffer(const std::vector<uint32_t> &attributeSizes);

	void addVertex(const std::vector<float> &vertexAttributes);

	void loadVertices(BufferAccessType accessType, BufferCallType callType);

	//offset of a attribute(not in bytes)
	uint32_t attributeOffset(uint32_t index) const;
	//total size of attributes(not in bytes)
	uint32_t stride() const;
	uint32_t vertexCount() const;
	GLOBJ_OVERRIDE(VertexBuffer)
};