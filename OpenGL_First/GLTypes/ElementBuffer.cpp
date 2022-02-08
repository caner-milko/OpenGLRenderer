#include <GLTypes/ElementBuffer.h>
ElementBuffer::ElementBuffer()
{
	glGenBuffers(1, &id);
	use();
}

void ElementBuffer::addTriangle(uint32_t index1, uint32_t index2, uint32_t index3)
{
	uint32_t indicesArr[3] = {index1, index2, index3};
	indices.insert(indices.end(), indicesArr, indicesArr + 3);
}

void ElementBuffer::addTriangles(const std::vector<uint32_t> &indices)
{
	this->indices.insert(this->indices.end(), indices.begin(), indices.end());
}

void ElementBuffer::loadElements(BufferAccessType accessType, BufferCallType callType)
{
	useIfNecessary();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_STREAM_DRAW + (int32_t)accessType + (int32_t)callType);
}

void ElementBuffer::drawElements(DrawType drawType)
{
	useIfNecessary();
	glDrawElements((int32_t)drawType, indices.size(), GL_UNSIGNED_INT, 0);
}

GLOBJ_DEFAULTS_MULTI_SPEC(ElementBuffer, glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, glDeleteBuffers)

GLOBJ_LAST_USED(ElementBuffer)