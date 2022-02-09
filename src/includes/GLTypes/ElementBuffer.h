#pragma once
#include <GLTypes/GLObject.h>
#include <vector>
#include <iostream>
#include <GLTypes/VertexBuffer.h>
#include <GLTypes/VertexArray.h>
class ElementBuffer : public GLObject
{
public:
	std::vector<uint32_t> indices;

	ElementBuffer();

	void addTriangle(uint32_t index1, uint32_t index2, uint32_t index3);
	void addTriangles(const std::vector<uint32_t> &indices);

	void loadElements(BufferAccessType accessType, BufferCallType callType);

	void drawElements(DrawType drawType);

	GLOBJ_OVERRIDE(ElementBuffer)
};