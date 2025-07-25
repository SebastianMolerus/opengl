#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include <string>
#include "glad/glad.h"
#include <vector>
#include <filesystem>
#include <string>

unsigned texture_from_file(std::filesystem::path const);

class texture
{
public:
	texture() = default;
	texture(std::string const& tex_name, int format = GL_RGB);
	texture& another_texture(std::string const& tex_name, int format = GL_RGB);
	void bind() const;
	unsigned texture_unit(std::string const& tex_name) const;
private:
	using texture_name_texture_id = std::pair<std::string, unsigned>;
	std::vector<texture_name_texture_id> m_ids;
};

#endif