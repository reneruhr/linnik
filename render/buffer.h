#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
namespace Render
{

auto SquareBuffer() ->	std::function<void(void)>;

class Buffer
{
private:
	unsigned int vao_, vbo_, ebo_;
	std::size_t size_;
public:
	template <class C>
	Buffer(const C& container) : size_(size(container))
	{	
		using vector = C::value_type;
		auto dim = sizeof(vector)/sizeof(float);		
	
		glGenVertexArrays(1, &vao_);
		glGenBuffers(1, &vbo_);

		glBindVertexArray(vao_);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_);
		glBufferData(GL_ARRAY_BUFFER, 
			     size_ * sizeof(vector),
			     container.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, dim, GL_FLOAT, GL_FALSE,
				      sizeof(vector), (void*)0);
		glBindVertexArray(0);
	}
	std::size_t Size() const { return size_; }
	unsigned int Vao() const { return vao_; }
};

auto PointsBuffer(const Buffer&) -> std::function<void(void)>;

}
