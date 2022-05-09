#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>

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
	void AddDrawCall(std::function<void(void)>);
	auto Width() const -> const int& { return width_;}	
	void Resize(int w, int h) { width_ = w; height_ = h; }
private:
	std::string title_;
	int width_, height_;
	float ratio_;
	GLFWwindow* window_;
	Cameras cameras_;
	std::vector<std::function<void(void)>> drawcalls_;
};

}
