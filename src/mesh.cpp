#include "mesh.h"
#include "shader.h"

static_assert(offsetof(vertex, position)	== 0,	"vertex order was changed");
static_assert(offsetof(vertex, normal)		== 12,	"vertex order was changed");
static_assert(offsetof(vertex, tex_coords)	== 24,	"vertex order was changed");

mesh::mesh(std::vector<vertex> vertices, std::vector<unsigned> indices, std::vector<unsigned> diffuse_textures,
	std::vector<unsigned> specular_textures)
	:m_vertices{ std::move(vertices) },
	m_indices{ std::move(indices) },
	m_diffuse_textures{std::move(diffuse_textures)},
	m_specular_textures{std::move(specular_textures)},
	m_vao{ "332", {&m_vertices[0].position.x, m_vertices.size() * sizeof(vertex) / sizeof(float)}}
{
	m_vao.ebo(m_indices);
}

void mesh::draw(shader const& s) const
{
	unsigned texture_unit{};
	for (unsigned diff_texture_id : m_diffuse_textures)
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		s.set_int("diffuse" + std::to_string(texture_unit), texture_unit);
		++texture_unit;
		glBindTexture(GL_TEXTURE_2D, diff_texture_id);
	}
	for (unsigned spec_texture_id : m_specular_textures)
	{
		glActiveTexture(GL_TEXTURE0 + texture_unit);
		s.set_int("specular" + std::to_string(texture_unit), texture_unit);
		++texture_unit;
		glBindTexture(GL_TEXTURE_2D, spec_texture_id);
	}
	m_vao.bind();
	glDrawElements(GL_TRIANGLES, (GLsizei)m_indices.size(), GL_UNSIGNED_INT, 0);
}