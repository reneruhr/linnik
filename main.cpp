// Compilation on MacOS:
//  $ g++ -std=c++20 *.cpp render/*.cpp -o out -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -framework OpenGL -lglew

#include "render.h"
#include "math.h"
#include <complex>
#include <chrono>

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
	window.AddDrawCall( [Points = DrawSpheres, 
				mvp = window.GetCamera()->PV(),
				color = NextColor(),
				last = std::chrono::steady_clock::now(),
				current_sphere = 0 ]() mutable { 
		auto time = std::chrono::duration<float>( std::chrono::steady_clock::now() - last);	
		if(time > 0.5s ){ 
			current_sphere++; 
			current_sphere %= (max_r+1);
			last = std::chrono::steady_clock::now();
			color = NextColor();
		}
		Points[current_sphere](mvp, color); 
		});

	window.Run();
	}
