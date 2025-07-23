#include "vao.h"
#include <glad/glad.h>

vao::vao(std::string const format, std::span<const float> v)
{
	glGenVertexArrays(1, &m_id);
	glBindVertexArray(m_id);

	unsigned int VBO{};
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, v.size_bytes(), v.data(), GL_STATIC_DRAW);

	unsigned stride{};
	for (auto const& l : format)
		stride += (l - '0');

	unsigned index{};
	unsigned val{};
	for (auto const& l : format)
	{
		glVertexAttribPointer(index, l - '0', GL_FLOAT,
			GL_FALSE, stride * sizeof(float), (void*)(val * sizeof(float)));
		glEnableVertexAttribArray(index);
		val += (l - '0');
		++index;
	}
}

void vao::ebo(std::span<const unsigned> data)
{
	unsigned ebo;
	glGenBuffers(1, &ebo);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size_bytes(), data.data(), GL_STATIC_DRAW);
}

void vao::bind() const
{
	glBindVertexArray(m_id);
}