#include "render.h"
#include "tools.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <array>


namespace Render 
{

auto Init(int,int,std::string) -> GLFWwindow*;
void ProcessInput(GLFWwindow* window);
void KeyCallback(GLFWwindow* window, int key, int code, int act, int mod);
void FBCallback(GLFWwindow* window, int w, int h);
void MouseCallback(GLFWwindow* window, double x, double y);
void ScrollCallback(GLFWwindow* window, double x, double y);

class Shader
{
	unsigned int vert_shader, frag_shader;
	unsigned int program_;

public:
	Shader(std::string, std::string);
	
	auto Id() -> unsigned int { return program_; }	
};

struct Camera
{
	using OrthFrame = std::array<float, 6>;

	glm::mat4 view_;
	glm::mat4 projection_;	
	glm::mat4 pv_;
	
	glm::vec3 front_ = {0.0f, 0.0f, 0.5f};
	glm::vec3 pos_ = {0.0f, 0.0f, -0.5f};
	glm::vec3 up_ = {0.0f, 1.0f, 0.0f};

	OrthFrame ortho_frame_;

	auto View() -> glm::mat4&
	{
		view_ = glm::lookAt(pos_, pos_ + front_, up_);	
		return view_; 
	}
	
	auto Projection(OrthFrame frame) -> glm::mat4&
	{
		ortho_frame_ = frame;
		projection_ = glm::ortho(frame[0], frame[1], 
				  frame[2], frame[3],
				  frame[4], frame[5]);
		return projection_;
	}

};


Window::Window(std::string title, float ratio , int width)
:
title_(title), width_(width), height_(ratio * width), ratio_(ratio)
{
	window_ = Init(width_, height_, title_);
	auto half_plane_view = std::make_shared<Camera>();
	half_plane_view->Projection({-1.f,1.f,0.f, ratio ,-1.f,1.f});
	cameras_.push_back(half_plane_view);
}

void Window::Run()
{
	while(!glfwWindowShouldClose(window_))
	{
		glClearColor(.3f,.2f,.2f,1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		ProcessInput(window_);
		glfwSwapBuffers(window_);
		glfwPollEvents();
	}
}

void Window::Exit()
{
	glfwTerminate();
}



auto SquareBuffer() -> auto(decltype)
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


auto DrawCircle(float radius, glm::vec3 center) ->decltype(auto)
{


}




auto Init(int w, int h, std::string title) -> GLFWwindow* 
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);	
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	auto window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);

	if(!window)
	{ 
		std::cout << "Failed Creating GLFW window\n";
		glfwTerminate();
		return nullptr;
	}

	glfwMakeContextCurrent(window);	
	glfwSetFramebufferSizeCallback(window, FBCallback);
	
	glewExperimental = GL_TRUE;
	if(auto e = glewInit(); GLEW_OK != e)
	{	
		std::cout << glewGetErrorString(e);
		glfwTerminate();
		return nullptr;
	}

	return window;
}

void ProcessInput(GLFWwindow* window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void KeyCallback(GLFWwindow* window, int key, int code, int act, int mod)
{
}

void FBCallback(GLFWwindow* window, int w, int h)
{
	glViewport(0,0,w,h);
}

void MouseCallback(GLFWwindow* window, double x, double y)
{
}

void ScrollCallback(GLFWwindow* window, double x, double y)
{
}	

Shader::Shader(std::string vert_name, std::string frag_name)
{
	vert_shader = glCreateShader(GL_VERTEX_SHADER);	
	auto  vert_shader_source = Tools::LoadFile(vert_name); 
	auto  vert_shader_source_ptr = vert_shader_source.c_str();
	std::cout << "Loaded Shader:\n" << vert_shader_source_ptr;
	glShaderSource(vert_shader, 1, &vert_shader_source_ptr , nullptr);
	glCompileShader(vert_shader);

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);	
	auto  frag_shader_source = Tools::LoadFile(frag_name); 
	auto  frag_shader_source_ptr =frag_shader_source.c_str();
	std::cout << "Loaded Shader:\n" << frag_shader_source_ptr;
	glShaderSource(frag_shader, 1, &frag_shader_source_ptr,  nullptr);
	glCompileShader(frag_shader);

	int  success;
	char infoLog[512];
	glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(vert_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(frag_shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	program_ = glCreateProgram();
	glAttachShader(program_, vert_shader);
	glAttachShader(program_, frag_shader);
	glLinkProgram(program_);

	glGetProgramiv(program_, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(program_, 512, NULL, infoLog);
		std::cout << "ERROR with Program\n" << infoLog << std::endl;	
	}

	glDeleteShader(frag_shader);
	glDeleteShader(vert_shader);
}
}
