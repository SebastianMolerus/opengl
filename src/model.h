#ifndef __MODEL_H__
#define __MODEL_H__

#include <string>
#include <vector>
#include "mesh.h"

class shader;
struct aiNode;
struct aiScene;
struct aiMesh;

class model
{
public:
	model(std::string name);
	void draw(shader const& s);
private:
	std::vector<mesh> m_meshes;
	std::string m_name;

	void process_scene(const aiScene* scene);
	mesh process_mesh(aiMesh* mesh, const aiScene* scene);
};

#endif