#include "model.h"
#include <filesystem>
#include <cassert>
#include "shader.h"
#include <iostream>
#include "texture.h"
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace
{
	std::filesystem::path const models_root{ MODELS_ROOT };
}

model::model(std::string name)
	:m_name{std::move(name)}
{
	assert(std::filesystem::exists(models_root / models_root / m_name));
	Assimp::Importer importer;
	const auto* scene{ importer.ReadFile((models_root / m_name).string(),
		aiProcess_Triangulate | aiProcess_FlipUVs)};

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASIMP::" << importer.GetErrorString() << "\n";
		throw std::runtime_error("ASSIMP_PROBLEM");
	}

	process_scene(scene);
}

void model::draw(shader const& s)
{
	s.bind();
	for (auto const& m : m_meshes)
		m.draw(s);
}

void model::process_scene(const aiScene* scene)
{
	for (unsigned i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* m{ scene->mMeshes[i] };
		m_meshes.push_back(process_mesh(m, scene));
	}
	assert(!m_meshes.empty());
}

mesh model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
	assert(mesh);
	assert(scene);

	std::vector<vertex> vertices;
	std::vector<unsigned> indices;
	std::vector<unsigned> diffuse_textures;
	std::vector<unsigned> specular_textures;

	// Vertices
	vertex v{};
	for (unsigned i = 0; i < mesh->mNumVertices; ++i)
	{
		v.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
		v.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };

		if (mesh->mTextureCoords[0]) // only first set, Assimp allows up to 8.
			v.tex_coords = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };

		vertices.push_back(v);
	}

	assert(!vertices.empty());

	// Indices
	for (unsigned i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned j = 0; j < face.mNumIndices; ++j)
			indices.push_back(face.mIndices[j]);
		assert(!indices.empty());
	}
	
	//Textures
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		for (unsigned i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); ++i)
		{
			aiString str;
			material->GetTexture(aiTextureType_DIFFUSE, i, &str);
			diffuse_textures.push_back(texture_from_file(models_root / str.C_Str()));
		}
		for (unsigned i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); ++i)
		{
			aiString str;
			material->GetTexture(aiTextureType_SPECULAR, i, &str);
			specular_textures.push_back(texture_from_file(models_root / str.C_Str()));
		}
	}

	return { vertices, indices, diffuse_textures, specular_textures };
}

