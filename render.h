#pragma once
#include <string>
#include <memory>
#include <vector>

struct GLFWwindow;

namespace Render 
{
struct Camera;
using Cameras = std::vector<std::shared_ptr<Camera>>;

class Window
{
public:
	Window(std::string, float, int);
	void Run();
	void Exit();
private:
	std::string title_;
	int width_, height_;
	float ratio_;
	GLFWwindow* window_;
	Cameras cameras_;
};



}
