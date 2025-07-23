#ifndef __MESH_H__
#define __MESH_H__

#include <glm/glm.hpp>
#include <vector>
#include "vao.h"

class shader;

struct vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coords;
};

class mesh
{
public:
	mesh(std::vector<vertex> vertices,
		std::vector<unsigned> indices,
		std::vector<unsigned> diffuse_textures,
		std::vector<unsigned> specular_textures);

	void draw(shader const& s) const;
private:
	std::vector<vertex>   m_vertices;
	std::vector<unsigned> m_indices;
	std::vector<unsigned> m_diffuse_textures;
	std::vector<unsigned> m_specular_textures;
	vao m_vao;
};

#endif