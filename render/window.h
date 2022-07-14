#pragma once

#include <vector>
#include <memory>
#include <string>
#include <functional>

struct GLFWwindow;

namespace Render
{

struct Camera;
using CameraPtr = std::shared_ptr<Camera>;
using Cameras = std::vector<CameraPtr>;
struct EventData
{
  double x,y;
};

class Window
{
public:
  Window(std::string title, float ratio, int width);
  void Run();
  void Exit();
  void AddDrawCall(std::function<void(void)>);
  void AddEventCall(std::function<void(const EventData&)>);
  auto Width() const -> const int& { return width_;}	
  void Resize(int w, int h);
  auto GetCamera() -> CameraPtr { return active_camera_; } 
  void MousePress(double x, double y);
  void UpdateViewport();
private:
  std::string title_;
  int width_, height_;
  float ratio_;
  GLFWwindow* window_;
  Cameras cameras_;
  CameraPtr active_camera_;
  std::vector<std::function<void(void)>> drawcalls_;
  std::vector<std::function<void(const EventData&)>> eventcalls_;
};

}
