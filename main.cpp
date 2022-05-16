// Compilation on MacOS:
//  $ g++ -std=c++20 *.cpp render/*.cpp -o out -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -framework OpenGL -lglew

#include "render.h"
#include "math.h"
#include <complex>

constexpr int p{3};	
constexpr int max_r{10};

int main() 
{
	using namespace Render;
	using namespace Math;

	auto width = 300;
	auto window = Window("CM Points.", 3.f, width );
	
	auto Triangle = [GeneralTriangle= DrawTriangle(), &w = window.Width()]() 
			{ GeneralTriangle(w, .02f); };
	window.AddDrawCall(std::move(Triangle));

	using G = GL2<int>;
	using C = std::complex<double>;

	
	C i{0.,1.};
	//auto spheres = HeckeSpheres<G,C,p,max_r>(i);
	window.Run();
}
