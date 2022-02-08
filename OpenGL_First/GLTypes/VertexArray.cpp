#include <glad/glad.h>
#include <GLTypes/VertexArray.h>


VertexArray::VertexArray(const VertexBuffer &buffer) : buffer(buffer)
{
	glGenVertexArrays(1, &id);
	use();
}

void VertexArray::drawArray(DrawType drawType)
{
	useIfNecessary();
	glDrawArrays((int32_t)drawType, 0, buffer.vertexCount());
}

GLOBJ_DEFAULTS_MULTI(VertexArray, glBindVertexArray, glDeleteVertexArrays)

GLOBJ_LAST_USED(VertexArray)