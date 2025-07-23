#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "texture.h"
#include <filesystem>
#include "glad/glad.h"
#include <unordered_map>

namespace {
	std::filesystem::path const textures_root{ TEXTURES_ROOT };

	std::unordered_map<std::filesystem::path, unsigned> textures_cache;
}

unsigned texture_from_file(std::filesystem::path const p)
{
	assert(std::filesystem::exists(p));

	if (unsigned id = textures_cache[p])
		return id; // already loaded from given path, return ID

	unsigned texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_set_flip_vertically_on_load(true);

	int width, height, nr_channels;
	unsigned char* data{ stbi_load(p.string().c_str(), &width, &height, &nr_channels, 0)};
	assert(data);

	GLenum format{};
	if (nr_channels == 1)
		format = GL_RED;
	else if (nr_channels == 3)
		format = GL_RGB;
	else if (nr_channels == 4)
		format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);

	++textures_cache[p];

	return texture;
}

texture::texture(std::string const& tex_name, int format)
{
	another_texture(tex_name, format);
}

texture& texture::another_texture(std::string const& tex_name, int format)
{
	assert(std::filesystem::exists(textures_root / tex_name));

	glActiveTexture(GL_TEXTURE0 + (GLenum)m_ids.size());
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
	for (GLenum i = 0; i < m_ids.size(); ++i)
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