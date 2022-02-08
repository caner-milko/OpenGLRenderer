#include <GLTypes/VertexBuffer.h>
#include <iostream>

VertexBuffer::VertexBuffer(const std::vector<uint32_t> &attributeSizes)
{
	this->attributeSizes = attributeSizes;
	glGenBuffers(1, &id);
	use();

}

void VertexBuffer::addVertex(const std::vector<float> &vertexInfo)
{
	if(vertexInfo.size() % stride() != 0)
	{
		std::cout << "Invalid vertex info" << std::endl;
		return;
	}

	vertices.insert(vertices.end(), vertexInfo.begin(), vertexInfo.end());
}

void VertexBuffer::loadVertices(BufferAccessType accessType, BufferCallType callType)
{
	useIfNecessary();
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STREAM_DRAW + (int32_t)accessType + (int32_t)callType);
	uint32_t strideSize = stride() * sizeof(float);

	for(int i = 0; i < attributeSizes.size(); i++)
	{
		glVertexAttribPointer(i, attributeSizes[i], GL_FLOAT, GL_FALSE, strideSize, (void *)(attributeOffset(i) * sizeof(float)));
		glEnableVertexAttribArray(i);
	}
}

uint32_t VertexBuffer::attributeOffset(uint32_t index) const
{
	uint32_t tot = 0;
	for(uint32_t i = 0; i < index && i < attributeSizes.size(); i++)
	{
		tot += attributeSizes[i];
	}
	return tot;
}

uint32_t VertexBuffer::stride() const
{
	return attributeOffset(attributeSizes.size());
}

uint32_t VertexBuffer::vertexCount() const
{
	return (uint32_t)vertices.size() / stride();
}

GLOBJ_DEFAULTS_MULTI_SPEC(VertexBuffer, glBindBuffer, GL_ARRAY_BUFFER, glDeleteBuffers)


GLOBJ_LAST_USED(VertexBuffer)