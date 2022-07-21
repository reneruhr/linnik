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
constexpr std::array<int,6> ps = {3, 3, 5, 11, 3, 3};
constexpr std::array<int,6> Ds = {-100271, -100391, -100411, -100447, -100511, -100547};
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

 
  constexpr auto n_cm = 10;
  constexpr auto D = 10000;
  auto [discs, cm_points] = CMPointsCollection<n_cm,D>();
  auto DrawCM = std::vector<std::function<void(MVP, Color, float)>>{};
  for(int r = 0; r<size(cm_points); ++r)
    DrawCM.push_back(DrawPoints(cm_points[r]));

  constexpr auto n_cm_total = 3*n_cm;  
  std::vector<std::string> cm_names_prep(n_cm_total);
  static const char* cm_names[n_cm_total];
  for(int i{0} ; i < n_cm; ++i){ 
	cm_names_prep[i] = "D = " + std::to_string(discs[i].first);
	cm_names_prep[i] +=  ". h = " + std::to_string(discs[i].second);
	cm_names[i] = cm_names_prep[i].c_str();
  }

  constexpr auto D2 = 100000;
  auto [discs2, cm_points2] = CMPointsCollection<n_cm,D2>();
  for(int r = 0; r<size(cm_points2); ++r)
    DrawCM.push_back(DrawPoints(cm_points2[r]));
  for(int i{0}; i < n_cm; ++i){ 
        cm_names_prep[n_cm+i] = "D = " + std::to_string(discs2[i].first);
	cm_names_prep[n_cm+i] +=  ". h = " + std::to_string(discs2[i].second);
	cm_names[n_cm+i] = cm_names_prep[i+n_cm].c_str();
      }

  constexpr auto D3 = 200000;
  auto [discs3, cm_points3] = CMPointsCollection<n_cm,D3>();
  for(int r = 0; r<size(cm_points3); ++r)
    DrawCM.push_back(DrawPoints(cm_points3[r]));
  for(int i{0}; i < n_cm; ++i){ 
	cm_names_prep[i+2*n_cm] = "D = " + std::to_string(discs3[i].first);
	cm_names_prep[i+2*n_cm] +=  ". h = " + std::to_string(discs3[i].second);
	cm_names[i+2*n_cm] = cm_names_prep[i+2*n_cm].c_str();
      }



  auto [discs, cm_points] = CMPointsCollection<n_cm,D>();
  auto DrawCM_NonCyclic = std::vector<std::function<void(MVP, Color, float)>>{};
  for(int r = 0; r<size(cm_points); ++r)

  Tools::for_range<0,n>([&Ds, &pts, &discs, &ps, &necklaces]<auto i>() {
      auto [discs, pts ,ps , necklaces] = NecklaceCollection<1,Ds[i]>();
      
      DrawCM_NonCyclic.push_back(DrawPoints(necklaces[0]));
    });

  static bool draw_on_click = true;
  static bool cycle_through= false;
  static int current_radius = 1;
  static int current_draw = -1;
  static bool active_gui= false;
  static float point_size = 8.f;

  static bool draw_hecke_sphere = true;
  static bool draw_cm_points = false;
  static bool draw_necklace = false;
  static int cm_active = 0;

  window.AddDrawCall( []()
    {
      ImGui::Begin("Point Controls");
      ImGui::SliderFloat("Point Size", &point_size, 0.f, 20.f);
      ImGui::Separator();
      ImGui::Text("Hecke Spheres");
      ImGui::Checkbox("Draw Hecke Sphere", &draw_hecke_sphere);
      active_gui = ImGui::IsWindowHovered(); 
      ImGui::Checkbox("Draw on Click", &draw_on_click);
      ImGui::SliderInt("Radius", &current_radius, 0, max_r);
      ImGui::Checkbox("Cycle through radii", &cycle_through);


      ImGui::Separator();
      ImGui::Text("CM Points");
      ImGui::Checkbox("Draw CM Points", &draw_cm_points);
      ImGui::ListBox("##D", &cm_active, cm_names, n_cm_total, 10);

      ImGui::Separator();
      ImGui::Text("CM Necklace");
      ImGui::End();
      //GUI::Demo();
    });


  window.AddEventCall( [](const EventData& xy){ std::cout << "Click at (" << xy.x << ", " << xy.y << ")\n"; return false;});
  window.AddEventCall( [&window, &DrawCM](const EventData& xy)
    { if(draw_on_click && not active_gui){
	C z{xy.x,xy.y};
	auto ball = HeckeBall<G,C,p,max_r>(z);
	auto float_ball = HeckeBallFloatConverter<C,p,max_r>(ball);	
	auto DrawSpheres = std::vector<std::function<void(MVP, Color, float)>>{};
	for(int r = 0; r<=max_r; ++r)
	  DrawSpheres.push_back(DrawPoints(float_ball[r]));
	auto call = [Points = DrawSpheres,
		         &DrawCM,
			mvp = window.GetCamera()->PV(),
      		      color = NextColor(),
      		      color2 = NextColor(),
		       last = std::chrono::steady_clock::now(),
	     current_sphere = 0
	  ]() mutable { 
	  if(not cycle_through and draw_hecke_sphere) Points[current_radius](mvp, color, point_size);
	  if(draw_cm_points) DrawCM[cm_active](mvp, color2, point_size);
	  if(cycle_through && draw_hecke_sphere){
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
