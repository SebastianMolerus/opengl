#ifndef __VAO__H
#define __VAO__H

#include <span>
#include <string>

class vao
{
public:
	vao(std::string const& format, std::span<const float> data);
	void ebo(std::span<const unsigned> data);
	void bind() const;

	vao(vao const&) = delete;
	vao& operator=(vao const&) = delete;
private:
	unsigned int m_id;
};

#endif // !__VAO__H