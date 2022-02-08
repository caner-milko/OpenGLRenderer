#include <GLTypes/Texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void configureTexture2DByType(const char *imgPath, bool vertical, TextureType inType, TextureType outType)
{

	stbi_set_flip_vertically_on_load(vertical);
	int width, height, nrChannels;
	unsigned char *data = stbi_load(imgPath, &width, &height, &nrChannels, 0);


	if(data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, (int32_t)outType, width, height, 0, (int32_t)inType, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Error loading second texture." << std::endl;
	}
	stbi_image_free(data);

}

Texture2D::Texture2D(const char *imgPath, bool vertical, TextureType inType, TextureType outType, TextureWrapping wrapping, TextureFiltering minFiltering, TextureFiltering maxFiltering)
{
	glGenTextures(1, &id);
	use();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int32_t)wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int32_t)wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int32_t)minFiltering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int32_t)maxFiltering);

	configureTexture2DByType(imgPath, vertical, inType, outType);
}

void Texture2D::changeFiltering(TextureFiltering minFiltering, TextureFiltering maxFiltering)
{
	useIfNecessary();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (int32_t)minFiltering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (int32_t)maxFiltering);
}

void Texture2D::changeWrapping(TextureWrapping wrapping, float *borderColor)
{
	useIfNecessary();
	if(wrapping == TextureWrapping::CLAMP_TO_BORDER)
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (int32_t)wrapping);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (int32_t)wrapping);
}

GLOBJ_DEFAULTS_MULTI_SPEC(Texture2D, glBindTexture, GL_TEXTURE_2D, glDeleteTextures)


GLOBJ_LAST_USED(Texture2D)