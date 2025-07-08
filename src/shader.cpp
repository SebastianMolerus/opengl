#include "shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <cassert>
#include <glm/gtc/type_ptr.hpp>

namespace
{
	std::filesystem::path const shaders_root{ SHADERS_ROOT };

	std::string read_file(std::filesystem::path const& p)
	{
		std::ifstream ifs{ p };
		std::stringstream ss;
		ss << ifs.rdbuf();
		return ss.str();
	}

	void check_shader_program_linking_error(unsigned int shader_program)
	{
		int success{};
		char info[512];
		glGetProgramiv(shader_program, GL_LINK_STATUS, &success);

		if (!success)
		{
			glGetProgramInfoLog(shader_program, sizeof(info), NULL, info);
			std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << info << "\n";
			throw std::runtime_error("ERROR::SHADER_PROGRAM::LINKING_FAILED");
		}
	}

	void check_shader_compilation_error(unsigned int shader, const char* type)
	{
		int success{};
		char info[512];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

		if (!success)
		{
			glGetShaderInfoLog(shader, sizeof(info), NULL, info);
			std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << info << "\n";
			throw std::runtime_error("ERROR::SHADER_PROGRAM::COMPILATION_FAILED");
		}
	}
}

shader::shader(std::string const& v, std::string const& f)
{
	std::filesystem::path const vertex{ shaders_root / v };
	std::filesystem::path const fragment{ shaders_root / f };

	if (!std::filesystem::exists(vertex) || !std::filesystem::exists(fragment))
		throw std::runtime_error("Path for vertex/fragment shader not exist.");

	std::string const vs{ read_file(vertex) };
	const char* vsc{ vs.c_str() };
	unsigned int const vertex_shader{ glCreateShader(GL_VERTEX_SHADER) };
	glShaderSource(vertex_shader, 1, &vsc, NULL);
	glCompileShader(vertex_shader);
	check_shader_compilation_error(vertex_shader, "VERTEX");

	std::string const fs{ read_file(fragment) };
	const char* fsc{ fs.c_str() };
	unsigned int const fragment_shader{ glCreateShader(GL_FRAGMENT_SHADER) };
	glShaderSource(fragment_shader, 1, &fsc, NULL);
	glCompileShader(fragment_shader);
	check_shader_compilation_error(fragment_shader, "FRAGMENT");

	m_id = glCreateProgram();
	glAttachShader(m_id, vertex_shader);
	glAttachShader(m_id, fragment_shader);
	glLinkProgram(m_id);
	check_shader_program_linking_error(m_id);

	glUseProgram(m_id);
}

void shader::bind()
{
	m_tex.bind();
	glUseProgram(m_id);
}

void shader::set_float(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void shader::set_int(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void shader::sampler_to_texture(std::string const& sampler_name, std::string const& texture_name, int format)
{
	m_tex.another_texture(texture_name, format);
	set_int(sampler_name, m_tex.texture_unit(texture_name));
}

void shader::set_mat(const std::string& name, glm::mat4 const& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}