#pragma once
#include <GLTypes/GLObject.h>
#include <GLTypes/VertexBuffer.h>
#include <GLTypes/ElementBuffer.h>
enum class DrawType
{
	TRIANGLES = GL_TRIANGLES, LINES = GL_LINES, POINTS = GL_POINTS
};
class VertexArray : public GLObject
{
public:

	VertexArray();

	void drawArray(const VertexBuffer &vertexBuffer, DrawType drawType);
	void drawElements(const ElementBuffer &elementBuffer, DrawType drawType);
	GLOBJ_OVERRIDE(VertexArray)
};