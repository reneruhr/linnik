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
  auto DrawSpheres = std::vector<std::function<void(MVP, Color, float)>>{};
  for(int r = 0; r<=max_r; ++r)
    DrawSpheres.push_back(DrawPoints(float_ball[r]));


  constexpr auto n_cm = 30;
  constexpr auto D = 10000;
  auto [discs, cm_points] = CMPointsCollection<n_cm,D>();
  auto DrawCM= std::vector<std::function<void(MVP, Color, float)>>{};
  for(int r = 0; r<size(cm_points); ++r)
    DrawCM.push_back(DrawPoints(cm_points[r]));

  std::vector<std::string> cm_names_prep(n_cm);
  static const char* cm_names[n_cm];
      for(int i = 0; i < n_cm; ++i){ 
	cm_names_prep[i] = "D= " + std::to_string(discs[i].first);
	cm_names_prep[i] +=  "h= " + std::to_string(discs[i].second);
	cm_names[i] = cm_names_prep[i].c_str();
      }
  static int cm_active = 0;

  static bool draw_on_click = true;
  static bool cycle_through= false;
  static int current_radius = 1;
  static int current_draw = -1;
  static bool active_gui= false;
  static float point_size = 8.f;
  window.AddDrawCall( []()
    {
      ImGui::Begin("Point Controls");
      ImGui::Text("Hecke Spheres");
      active_gui = ImGui::IsWindowHovered(); 
      ImGui::Checkbox("Draw on Click", &draw_on_click);
      ImGui::SliderInt("Radius", &current_radius, 0, max_r);
      ImGui::Checkbox("Cycle through radii", &cycle_through);

      ImGui::SliderFloat("Point Size", &point_size, 0.f, 20.f);

      ImGui::Text("CM Points");
      ImGui::ListBox("D", &cm_active, cm_names, n_cm, 4);

      ImGui::End();
      //GUI::Demo();
    });


  window.AddEventCall( [](const EventData& xy){ std::cout << "Click at (" << xy.x << ", " << xy.y << ")\n"; return false;});
  window.AddEventCall( [&window](const EventData& xy)
    { if(draw_on_click && not active_gui){
	C z{xy.x,xy.y};
	auto ball = HeckeBall<G,C,p,max_r>(z);
	auto float_ball = HeckeBallFloatConverter<C,p,max_r>(ball);	
	auto DrawSpheres = std::vector<std::function<void(MVP, Color, float)>>{};
	for(int r = 0; r<=max_r; ++r)
	  DrawSpheres.push_back(DrawPoints(float_ball[r]));
	auto call = [Points = DrawSpheres, 
			     mvp = window.GetCamera()->PV(),
		     color = NextColor(),
		     last = std::chrono::steady_clock::now(),
		     current_sphere = 0 
	  ]() mutable { 
	  if(not cycle_through) Points[current_radius](mvp, color, point_size);
	  else {
	    auto time = std::chrono::duration<float>( std::chrono::steady_clock::now() - last);	
	    if(time > 0.5s ){ 
	      current_sphere++; 
	      current_sphere %= (max_r+1);
	      last = std::chrono::steady_clock::now();
	      color = NextColor();
	    }
	    Points[current_sphere](mvp, color, point_size); 
	  }
	};
      if(current_draw != -1) window.ChangeDrawCall(current_draw, std::move(call));
      else current_draw = window.AddDrawCall(std::move(call));
      }
      return false;
    });

  window.Run();
}
