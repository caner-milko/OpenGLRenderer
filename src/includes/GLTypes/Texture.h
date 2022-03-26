#pragma once

#include <glad/glad.h>
#include <iostream>
#include <GLTypes/GLObject.h>
enum class TextureWrapping
{
	REPEAT = GL_REPEAT, MIRRORED_REPEAT = GL_MIRRORED_REPEAT, CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE, CLAMP_TO_BORDER = GL_CLAMP_TO_BORDER
};
enum class TextureFiltering
{
	NONE = -1, LINEAR = GL_LINEAR, NEAREST = GL_NEAREST, LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
};

class Texture2D : public GLObject
{
public:
	std::string path;
	Texture2D(const char *imgPath, TextureWrapping wrapping = TextureWrapping::REPEAT, TextureFiltering minFiltering = TextureFiltering::LINEAR_MIPMAP_LINEAR, TextureFiltering maxFiltering = TextureFiltering::LINEAR);

	void changeWrapping(TextureWrapping wrapping, float *borderColor);
	void changeFiltering(TextureFiltering minFiltering, TextureFiltering maxFiltering);
	GLOBJ_OVERRIDE(Texture2D)
};