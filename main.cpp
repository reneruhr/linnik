// Compilation on MacOS:
//  ~/g++11 -std=c++20 *.cpp render/*.cpp tools/*.cpp render/imgui/*.cpp -o linnik -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -framework OpenGL -lglew
// Linux:
// g++ -std=c++20 -Wall *.cpp render/*.cpp tools/*.cpp render/imgui/*.cpp -o linnik -lglfw -lGLEW -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm 
#include "render/render.h"
#include "render/gui.h"
#include "math.h"
#include <complex>
#include <chrono>
#include <iostream>
constexpr int p{3};	
constexpr int max_r{9};

int main() 
{
  using namespace Render;
  using namespace Math;
  using namespace std::chrono_literals;

  auto width = 300;
  auto window = Window("CM Points.", 3.f, width );
	
  window.AddDrawCall( 
    [Triangle = DrawTriangle(), &w = window.Width()]()
      { 
	Triangle(w, .02f);
      });

  using G = GL2<int>;
  using C = std::complex<double>;

	
  C i{0.,1.};
  auto ball = HeckeBall<G,C,p,max_r>(i);
  auto float_ball = HeckeBallFloatConverter<C,p,max_r>(ball);	
  auto DrawSpheres = std::vector<std::function<void(MVP, Color)>>{};
  for(int r = 0; r<=max_r; ++r)
    DrawSpheres.push_back(DrawPoints(float_ball[r]));


  static bool draw_on_click = true;
  static bool cycle_through= false;
  static int current_radius = 1;
  static int current_draw = -1;
  static bool active_gui= false;
  window.AddDrawCall( []()
    {
      ImGui::Begin("Point Controls");
      ImGui::Text("Hecke Spheres");
      active_gui = ImGui::IsWindowHovered(); 
      ImGui::Checkbox("Draw on Click", &draw_on_click);
      ImGui::SliderInt("Radius", &current_radius, 0, max_r);
      ImGui::Checkbox("Cycle through radii", &cycle_through);


      ImGui::Text("CM Points");
/*
      std::vector<std::string> radii(r_max+1);
      const char* imgui_radii[r_max+1];
      for(int i = 0; r<= r_max; ++i){ 
	radii[i] = std::to_string(i);
	imgui_radii[i] = radii[i].c_str();
      }
      static int radius_current = 1;
      ImGui::ListBox("Sphere Radius", &radius_current, imgui_radii, r_max+1, 4);
*/
      ImGui::End();
      //GUI::Demo();
    });


  window.AddEventCall( [](const EventData& xy){ std::cout << "Click at (" << xy.x << ", " << xy.y << ")\n"; return false;});
  window.AddEventCall( [&window](const EventData& xy)
    { if(draw_on_click && not active_gui){
	C z{xy.x,xy.y};
	auto ball = HeckeBall<G,C,p,max_r>(z);
	auto float_ball = HeckeBallFloatConverter<C,p,max_r>(ball);	
	auto DrawSpheres = std::vector<std::function<void(MVP, Color)>>{};
	for(int r = 0; r<=max_r; ++r)
	  DrawSpheres.push_back(DrawPoints(float_ball[r]));
	auto call = [Points = DrawSpheres, 
			     mvp = window.GetCamera()->PV(),
		     color = NextColor(),
		     last = std::chrono::steady_clock::now(),
		     current_sphere = 0 
	  ]() mutable { 
	  if(not cycle_through) Points[current_radius](mvp, color);
	  else {
	    auto time = std::chrono::duration<float>( std::chrono::steady_clock::now() - last);	
	    if(time > 0.5s ){ 
	      current_sphere++; 
	      current_sphere %= (max_r+1);
	      last = std::chrono::steady_clock::now();
	      color = NextColor();
	    }
	    Points[current_sphere](mvp, color); 
	  }
	};
      if(current_draw != -1) window.ChangeDrawCall(current_draw, std::move(call));
      else current_draw = window.AddDrawCall(std::move(call));
      }
      return false;
    });

  window.Run();
}
