#pragma once
#include <GLTypes/GLObject.h>
#include <vector>
#include <iostream>
#include <GLTypes/VertexBuffer.h>
class ElementBuffer : public GLObject
{
public:
	const uint32_t indicesSize;

	ElementBuffer(const std::vector<uint32_t> &indices, BufferAccessType accessType, BufferCallType callType);

	/*void addTriangle(uint32_t index1, uint32_t index2, uint32_t index3);
	void addTriangles(const std::vector<uint32_t> &indices);

	void loadElements(BufferAccessType accessType, BufferCallType callType);*/

	GLOBJ_OVERRIDE(ElementBuffer)
};