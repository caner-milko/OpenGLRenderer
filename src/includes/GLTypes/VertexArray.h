#pragma once
#include <GLTypes/GLObject.h>
#include <GLTypes/VertexBuffer.h>
enum class DrawType
{
	TRIANGLES = GL_TRIANGLES, LINES = GL_LINES, POINTS = GL_POINTS
};
class VertexArray : public GLObject
{
public:
	const VertexBuffer &buffer;

	VertexArray(const VertexBuffer &buffer);

	void drawArray(DrawType drawType);
	GLOBJ_OVERRIDE(VertexArray)
};