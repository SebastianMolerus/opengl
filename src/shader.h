#ifndef __SHADER_H__
#define __SHADER_H__

#include <string>
#include "glm/mat4x4.hpp"
#include "texture.h"

class shader
{
public:
	shader(std::string const& vertex, std::string const& fragment);
	void bind() const;

	void sampler_to_texture(std::string const& sampler_name, std::string const& texture_name, int format = GL_RGB);
	void set_float(const std::string& name, float value) const;
	void set_int(const std::string& name, int value) const;
	void set_mat(const std::string& name, glm::mat4 const& value) const;
	void set_vec3(const std::string& name, glm::vec3 const& value) const;
	void set_vec3(const std::string& name, float, float, float) const;
private:
	unsigned m_id;
	texture m_tex;
};

#endif