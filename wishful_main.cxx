#include "linnik.h"

// Creates an OpenGL window depicting the upper halfplane.
class H;

// Can be given to H to tile it.
class Tiling;

// Subgroup of PSL(R) that can be used to create a tiling.
// optional argument unsigned int p creates the principal congruence lattice of SL2(Z)
class Gamma;

// (a,b,c) <-> q(x,y)=ax^2 + bxy + cy^2
// disc = b^2-4ac
class BinaryQuadraticForm;


auto BinaryQuadraticForm::Point() -> Point;
auto BinaryQuadraticForm::Ideal() -> Ideal;



IsDiscriminant = 0 mod 4  or 1 mod 4
IsFundamental = SquareFree or d/4 ( SquareFree and =2,3 mod 4) 

auto KroneckerSymbol( auto d, auto p) -> int;

auto pSplits = [](auto d, auto p) { return KroneckerSymbol(d,p) == 1; };

auto IsReduced(BinaryQuadraticForm q) -> bool; 

// Point in Upper Halfspace
template<class Scalar>
class Point;

template<class Scalar>
using Points = Container<Point<Scalar>>:

template <int p, int r, class Scalar>
auto HeckeSphere(Point point) -> Points;

template <int p, class Scalar>
using HeckeNeighbours = HeckeSphere<p,1,Scalar>;



using CMPoint = BinaryQuadraticForm;

auto Reduce(auto CMPoint) -> decltype(auto);

// D<0
template <int D>
class Ideal;

Ideal::Lattice() -> Lattice;

class Lattice;
Lattice::Lattice(Ideal)


template <int D>
auto PrimeDecomposition(int p) -> std::tuple<Ideal,Ideal>;

auto Multiply(Ideal, Ideal) -> Ideal;


// Gives k for which Ideal^k is principal
auto Order(Ideal) -> int

auto Flow(CMPoint point, Ideal P) -> Point;
auto Flow(CMPoint point, Ideal P, int k) -> Point;



template <int D>
struct CMPoints
{
 Container<CMPoint> cmpoints;
 using Disc = D;
}

template <int D>
auto Trivial() -> std::tuple<CMPoint, CMPoint>;

auto Necklace(CMPoint, Ideal P) -> CMPoints;

template <int p>
auto Necklace(CMPoint) -> CMPoints;

auto FindCMPoints(int D) -> CMPoints;

auto main() -> int
{
	using namespace linnik;


/*	auto gamma = Gamma{1u};
	auto tiling = Tiling{gamma};
	auto window = H{tiling};
*/

	auto screens = TileScreenHorizontally(3);
	
	Hs = screens | H;	
	

	auto Ps = Prime | OneModFour ;
	auto Ds = NegativeInteger | IsFundamental;
	
	
	auto ten_Ds = Ds | Random(1, 1'000'000)::Take(10);

	auto cm_points = ten_Ds | FindCMPoints;

	
	auto window = H[0];
	
	auto DrawCMPointsCyclically = [cm_points&, window&](){
		cm_points | 
		CycleThrough(window, 3s, 
			[](auto& pts){ std::cout << "CMPoints of discriminant " << pts::Disc << '\n'; } );
	};

	auto cm_points_trivial = ten_Ds | Trivial | TakeFirst;

	auto p = Ps | Take(1);

	auto DrawHeckeSpheres = [cm_points_trivial&, window&, p] (int radius)
	{
		spheres = Take(1,radius) | ( cm_points_trivial | HeckeSphere<p>(radius) ) ;
		spheres | CycleThrough( .. );
	}; 

	auto necklaces = cm_points_trivial | Necklace;
		
	auto DrawNecklaces= [necklaces&, window&] (int radius)
	{
		necklaces | CycleThrough( .. );
	}; 

	

}
