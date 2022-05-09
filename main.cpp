// Compilation on MacOS:
//  $ g++ -std=c++20 *.cpp render/*.cpp -o out -I/opt/homebrew/include -L/opt/homebrew/lib -lglfw -framework OpenGL -lglew

#include "render.h"

auto main() -> int
{
	using namespace Render;

	auto width = 300;
	auto window = Window("CM Points.", 3.f, width );
	
	auto Triangle = [GeneralTriangle= DrawTriangle(), &w = window.Width()]() 
			{ GeneralTriangle(w, .02f); };
	window.AddDrawCall(std::move(Triangle));
	window.Run();
}
