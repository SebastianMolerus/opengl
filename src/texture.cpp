#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "texture.h"
#include <filesystem>
#include "glad/glad.h"

namespace {
	std::filesystem::path const textures_root{ TEXTURES_ROOT };
}

texture::texture(std::string const& tex_name, int format)
{
	another_texture(tex_name, format);
}

texture& texture::another_texture(std::string const& tex_name, int format)
{
	if (!std::filesystem::exists(textures_root / tex_name))
		throw std::runtime_error("Cannot find texture.");

	glActiveTexture(GL_TEXTURE0 + m_ids.size());
	m_ids.push_back({ tex_name, -1 });
	glGenTextures(1, &m_ids.back().second);
	glBindTexture(GL_TEXTURE_2D, m_ids.back().second);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nr_channels;
	unsigned char* data{ stbi_load((textures_root / tex_name).string().c_str(), &width, &height, &nr_channels, 0) };

	if (!data)
		throw std::runtime_error("stbi_load failure.");

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	return *this;
}

void texture::bind() const
{
	for (size_t i = 0; i < m_ids.size(); ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_ids[i].second);
	}
}

unsigned texture::texture_unit(std::string const& tex_name) const
{
	for (unsigned i = 0; i < m_ids.size(); ++i)
	{
		if (m_ids[i].first == tex_name)
			return i;
	}
	return -1;
}