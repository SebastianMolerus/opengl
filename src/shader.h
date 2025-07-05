#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>
#include "glm/mat4x4.hpp"

class texture;

class shader
{
public:
	shader(std::string const& vertex, std::string const& fragment);
	shader(std::string const& vertex, std::string const& fragment, texture const* tex);
	void bind();

	void sampler_to_texture(std::string const& sampler_name, std::string const& texture_name) const;
	void set_float(const std::string& name, float value) const;
	void set_int(const std::string& name, int value) const;
	void set_mat(const std::string& name, glm::mat4 const& value) const;
private:
	unsigned m_id;
	texture const* m_tex{ nullptr };
};

#endif