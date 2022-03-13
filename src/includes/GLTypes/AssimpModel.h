#pragma once
#include <string>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GLTypes/RenderObject.h>
#include <GLTypes/Mesh.h>
#include <GLTypes/Texture.h>

struct AssimpTexture
{
	Texture2D *texture;
	std::string type;
	std::string path;
};

struct AssimpMesh
{
	const std::vector<Vertex> vertices;
	const std::vector<uint32_t> indices;
	const std::vector<AssimpTexture> textures;

	AssimpMesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices, std::vector<AssimpTexture> textures) : vertices(vertices), indices(indices), textures(textures)
	{
	}
};

class AssimpModel
{
public:
	std::vector<AssimpMesh> meshes;
	std::vector<AssimpTexture> texturesLoaded;
	std::string directory;

	AssimpModel(const char *path);

private:
	void loadModel(std::string path);
	void processNode(aiNode *node, const aiScene *scene);
	AssimpMesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<AssimpTexture> loadMaterialTextures(aiMaterial *mat, aiTextureType type,
		std::string typeName);
};