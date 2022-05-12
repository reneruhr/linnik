#include "render.h"
#include "tools.h"
#include "render/shader.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <array>


namespace Render 
{

auto SquareBuffer() -> decltype(auto)
{
	auto square = std::array<float,8>
		{
			1.f,  1.f,
			1.f, -1.f,
			-1.f, -1.f, 
			-1.f,  1.f 
		}; 
	
	auto indices = std::array<unsigned int, 6>
		{
			0,1,3,1,2,3
		};

	unsigned int vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);
	
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square.data(), GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	return [vao,vbo,ebo]()
	{
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	};	
}

auto DrawTriangle() -> std::function<void(int, float)> 
{
	auto shader = Shader("shaders/circle.vert", "shaders/triangle.frag");
	auto DrawCall = SquareBuffer();
	
	return [p = shader.Id(), Draw = DrawCall](int w, float t)
	{
		glUseProgram(p);	
		glUniform1i( glGetUniformLocation(p, "width"), w);
		glUniform1f( glGetUniformLocation(p, "line_thickness"), t);
		Draw();
	};
}

}
