//macos: clear && ~/g++11 test.cpp external_libs/catch2/*.cpp -o mathtest -std=c++20 -Wno-psabi && ./mathtest

#define CATCH_CONFIG_MAIN
#include "external_libs/catch2/catch_amalgamated.hpp"
#include "../math.h"
#include <sstream>
#include <algorithm>
#include <vector>
#include <complex>
#include <limits>
#include <iomanip>

using namespace std::literals::complex_literals;

TEST_CASE("Primes", "[BinaryForms]")
{
	using namespace Math;
	REQUIRE( IsPrime(1063) == true );
	REQUIRE( IsPrimeAndFundamental(-5923) == true );
}

TEST_CASE("GCD", "[Arithmetic]")
{
using namespace Math;

	REQUIRE( Gcd(12,6) == 6 );
	REQUIRE( Gcd(3*8*9,10*7*8) == 8 );
	REQUIRE( Gcd(11,7) == 1 );
	REQUIRE( Gcd(11,7,5) == 1 );
	REQUIRE( Gcd(11,7,14,23) == 1 );
	REQUIRE( Gcd(3,3*5,3*5*7) == 3 );
	REQUIRE( Gcd(3*11,3*5*11,3*5*7*11) == 3*11 );
}

TEST_CASE("SquareRootMod", "[Arithmetic]")
{
using namespace Math;
	auto tester =[](int x, int p)
	{ 
		auto res = *SquareRootMod(ModPow(x,2,p), p);
		return  ( (res-x) % p == 0 ) || ( (res+x) % p == 0 );
	};
	REQUIRE( tester(81,5) == true ); 
	REQUIRE( tester(101,11) == true ); 
	REQUIRE( tester(23424,101) == true ); 
}

TEST_CASE("Integer power mod p", "[Arithmetic]"){
using namespace Math;
	REQUIRE ( (91*91*91 ) % 5 == ModPow(91,3,5) );
	REQUIRE ( (3*3*3*3*3*3) % 2 == ModPow(3,6,2) );
}

TEST_CASE("Kronecker", "[Arithmetic]"){
using namespace Math;
	constexpr auto res  = std::array<int, 5>
		{ 1-Kronecker(2,7),
		  1-Kronecker(7,9),
		  -1-Kronecker(8,11),
		  0-Kronecker(14,12),
		  1-Kronecker(18,7) };
	REQUIRE( res == std::array<int,5>{} );
}

TEST_CASE("Extended Euclid", "[Arithmetic]"){
using namespace Math;
	constexpr auto res  = ExtendedEuclid(48,18);
	constexpr auto u = std::get<0>(res);
	constexpr auto v = std::get<1>(res);
	constexpr auto d = std::get<2>(res);
	REQUIRE( d == 6 );
	REQUIRE( u*48 + v* 18 == d); 	
}

TEST_CASE("Constexpr Square", "[Arithmetic]"){
{using namespace Math;
	constexpr auto y = Square(12345*12345);
	auto close =  ( abs((y-12345)) <= 0.000001 ); 
	REQUIRE( close ); 
}}

TEST_CASE("FastPow", "[Arithmetic]"){
{using namespace Math;
	constexpr auto r1 = FastPow(2,0);
	constexpr auto r2 = FastPow(2,1);
	constexpr auto r3 = FastPow(2,2);
	constexpr auto r4 = FastPow(2,3);
	constexpr auto s  = FastPow(83l,5);
	REQUIRE(  r1 == 1 ); 
	REQUIRE(  r2 == 2 ); 
	REQUIRE(  r3 == 4 ); 
	REQUIRE(  r4 == 8 ); 
	REQUIRE(  s  == 83l*83*83*83*83 ); 
}}

TEST_CASE("IntPow", "[Arithmetic]"){
{using namespace Math;
	REQUIRE( IntPow(2,0) == 1 ); 
	REQUIRE( IntPow(2,1) == 2 ); 
	REQUIRE( IntPow(2,2) == 4 ); 
}}

TEST_CASE("Ceil and Floor", "[Arithmetic]"){
	auto x = 3.5;
	using namespace Math;
	REQUIRE( IntCeil(x)==4 ); 	
	REQUIRE( IntFloor(x)==3 ); 	
	auto z = 1.;
	REQUIRE( IntFloor(z)==1 ); 	
	REQUIRE( IntCeil(z)==1 ); 	
}

TEST_CASE( "Create Identity ", "[GL]" ) {
{using namespace Math;
	GL2<int> mat;

	REQUIRE( mat.At(0,0) == 1 );
	REQUIRE( mat.At(1,1) == 1 );
}}

TEST_CASE( "Constructor", "[GL]" ) {
{using namespace Math;
	GL2<int> mat(1,2,3,4);
	REQUIRE( mat.At(0,1) == 3 );
}}

TEST_CASE( "Access", "[GL]") {
{using namespace Math;
	GL2<int> mat(1,2,3,4);
	REQUIRE( a(mat) == 1 );
	REQUIRE( b(mat) == 3 );
	REQUIRE( c(mat) == 2 );
	REQUIRE( d(mat) == 4 );
}}


TEST_CASE( "Print Test", "[GL]" ) {
{using namespace Math;
	GL2<int> mat(1,2,3,4);

	std::stringstream os;
	os << mat;

	REQUIRE( os.str()  == "1, 3\n2, 4\n");
}}

 TEST_CASE("Comparison", "[GL]"){
{using namespace Math;
	GL2<int> mat;
	GL2<int> mat_same;
	GL2<int> mat2(2,0,0,2);
	REQUIRE( mat == mat_same); 
}}

TEST_CASE( "Hecke Neighbours", "[Hecke]" ) {
{using namespace Math;
	constexpr int p =3;
	auto nbs = HeckeNeighbours<GL2<int>,p>{};
	auto n0 = GL2<int>(p,0,0,1);
	auto n0dag = nbs[0];
	REQUIRE(  n0 == n0dag );
}}

TEST_CASE( "Hecke UpNeighbour", "[Hecke]" ) {
{using namespace Math;
	constexpr int p =3;
	auto nb = HeckeUpNeighbour<GL2<int>,p,2>();
	auto n0 = GL2<int>(1,0,0,p*p);
	REQUIRE(  n0 == nb );
}}

TEST_CASE( "Hecke DownNeighbour", "[Hecke]" ) {
{using namespace Math;
	constexpr int p =3, r=1;
	auto nbs = HeckeDown<GL2<int>,p,r>();
	REQUIRE(  size(nbs)==p );
	auto n0 = GL2<int>(p,0,p-1,1);
	auto n = nbs[p-1];
	REQUIRE(  n==n0 );


	constexpr int s=2;
	auto nbs2 = HeckeDown<GL2<int>,p,s>();
	REQUIRE(  size(nbs2)== p*p + 1*(p-1) );
}}

TEST_CASE( "Hecke 2-Neighbours Print", "[Hecke]" ) {
{using namespace Math;
	constexpr int p = 3;
	auto nbs = HeckeSphere<GL2<int>,p,2>();
	
	std::vector<GL2<int>> my_nbs = { {p,0,1,p}, {p*p,0,1,1}, {1,0,0,p*p} };
	for(auto& n : my_nbs)
		REQUIRE( std::find(begin(nbs),end(nbs),n) != end(nbs) );
	REQUIRE( size(nbs) == (p+1)*p );
}}


TEST_CASE( "Complex Number", "[Complex]" ) {
	auto z = std::complex<double>(1i); 
	auto w = std::complex<double>(1i +1.); 
	REQUIRE(  ( real(z)==0 ) );
	REQUIRE( ( imag(z) == 1 ));
	REQUIRE( ( real(w) == 1 )); 
}

TEST_CASE("Mobius", "[Complex]"){
	using namespace Math;
	auto g = GL2<int>(2,1,1,1);
	auto z = 1.i;
	auto zg = Mobius(z,g);	
	REQUIRE ( zg == (1.i-3.) / 5. ) ;
}

TEST_CASE("Mobius ReductionTranslation", "[Complex]"){
	using namespace Math;
	auto zs = std::vector<std::complex<double>>{ {1.,1.}, {-0.5,1.}, {0.5,2.}, {-3., 0.5} };
	auto zs_red = std::vector<std::complex<double>>{ {0.,1.}, {0.5,1.}, {0.5,2.}, {-0., 0.5} };
	for(int i = 0; i<4; ++i)
		REQUIRE ( ReductionTranslation(zs[i]) == zs_red[i] ) ;
}


TEST_CASE("RandomSequenceGenerator", "[Random]"){
	using namespace Math;
	auto gen = RandomSequenceGenerator(1u,2);
	std::vector<int> results{0, 1, 1, 1, 0, 
				 0, 0, 1, 0, 0, 
				 0, 0, 0, 0, 0, 
				 1, 0, 1, 1, 1, 
				 0, 0, 1, 1, 0};
	auto res = begin(results);	
	for(int j =0; j<5; ++j) {
		auto sequence = gen.Get(5);
		for(auto i : sequence) 
			REQUIRE( *(res++) == i );
	}
}


TEST_CASE("RandomWord", "[Random]"){
	
	using namespace Math;
	using Gamma = GL2<int>;
	auto u = Gamma(1,0,1,1); 
	auto w = Gamma(0,-1,1,0); 
	std::vector<Gamma> results {	
			{1, 0,
			1, 1},
			{-2, -3,
			-1, -2},
			{1, 5,
			0, 1},
			{1, 0,
			-1, 1},
			{-1, -3,
			0, -1}};
	//Gamma in column major format,so above format is misleading:
	auto res = begin(results);
	std::for_each(res, end(results), [](Gamma& g) { g=Transpose(g);});

	for(int j =0; j<5; ++j) {
		auto g = RandomWord<Gamma, 2>(5,{u,w});
		REQUIRE( *(res++) == g );
	}
	
}


TEST_CASE("Mobius Reduction", "[Complex]"){
	using namespace Math;
	using Gamma = GL2<int>;
	const double pi = std::acos(-1);
	const std::complex<double> I(0,1);
	auto zs = std::vector<std::complex<double>>
		  { {0.,1.}, {0.3,1.2}, {0.5,20.}, {-.2,500. }, exp(I*(pi/2+0.3)) };
	auto zs_moved = zs;

	auto u = Gamma(1,0,1,1); 
	auto w = Gamma(0,-1,1,0); 
	auto move_z = [u,w](auto& z)
	{
		auto g =RandomWord<Gamma,2>(25, {u,w});
		//std:: cout << g;
		//std::cout << z << " ->  ";
		z = Reduction(z);
		//std::cout << z << " ->  ";
		z = Mobius(z, g);
		//std::cout << z << " ->  ";
		z = Reduction(z);
		//std::cout << z << '\n';
	};		
	std::for_each(begin(zs_moved),end(zs_moved), move_z);

	auto ApproxEqual = [](auto z, auto w) {
			auto re = abs(real(z)-real(w));
			auto im = abs(imag(z)-imag(w));
			auto eps = 0.000001; 
			if(im <= eps)
				if (re <=eps) return true;
				else if(abs(re-1.) <= eps) return true;
				else if(  (abs(abs(z)-1) <eps)
					&&(abs(abs(w)-1) <eps)  )
					return ( abs(real(z)+real(w)) < 2*eps); 
			return false;
		};
	for(int i = 0; i< size(zs); ++i)
		REQUIRE(  ApproxEqual(zs[i], zs_moved[i]));
}
/*
TEST_CASE("HeckeSphere on Surface", "[Hecke]"){
	using namespace Math;
	using G = GL2<int>;
	using C = std::complex<double>;
	C i(0,1);
	constexpr int p{5};	
	constexpr int r{1};	

	auto nbs = HeckeSphereMobius<G,C,p,r>(i);	
	for(auto a : nbs) std::cout << a << ", ";
	REQUIRE(true);	
}
*/

TEST_CASE("HeckeBall Size", "[Hecke]"){
	using namespace Math;
	constexpr int p{5};
	constexpr int r{3};
	auto num = HeckeBallSize<p,r>();
	auto exp_num = (p+1)*(1+p+p*p)+1;
	REQUIRE( num == exp_num);	
}

TEST_CASE("HeckeBall", "[Hecke, Mobius]"){
	using namespace Math;
	using C = std::complex<double>;
	using G = GL2<int>;
	constexpr int p{2};
	constexpr int r{2};
	C z{0.,1.};
	
	auto ball = HeckeBall<G,C,p,r>(z);

	auto CompareSphere = [](auto sphere1, auto sphere2){
		auto a= begin(sphere1);
		bool ok = true;
		for(auto& b : sphere2 ) {
			ok = ok && (b == *(a++));		
		}
		return ok;
	};
	auto s0 = CompareSphere(HeckeSphereMobius<G,C,p,0>(z), ball.GetSphere<0>());
	auto s1 = CompareSphere(HeckeSphereMobius<G,C,p,1>(z), ball.GetSphere<1>());
	auto s2 = CompareSphere(HeckeSphereMobius<G,C,p,2>(z), ball.GetSphere<2>());
	REQUIRE( s0== true );	
	REQUIRE( s1== true );	
	REQUIRE( s2== true );	
}


TEST_CASE("HeckeBall Large", "[Hecke, Mobius]"){
	using namespace Math;
	using C = std::complex<double>;
	using G = GL2<int>;
	constexpr int p{3};
	constexpr int r{9}; // r=10 crashes at runtime
	C z{0.,1.};
	
	REQUIRE( 1 < HeckeBallSize<p,r>() );
	REQUIRE( HeckeSphereMobius<G,C,p,r>(z)[1].imag() > 0. );
	REQUIRE( HeckeBall<G,C,p,r>(z).ball_[0].real() < 2.);
}

TEST_CASE("HeckeBall Float", "[Datatype]"){
	using namespace Math;
	using C = std::complex<double>;
	using F = std::complex<float>;
	using G = GL2<int>;
	constexpr int p{3};
	constexpr int r{5}; // r=10 crashes at runtime
	C z{0.,1.};

	auto ball = HeckeBall<G,C,p,r>(z);
	auto float_ball = HeckeBallFloatConverter<C,p,r>(ball);	
	float eps = 0.0001;
	for(int i = 0; i < size(ball.GetSphere<1>()); ++i) {
		auto e = 
	static_cast<std::complex<float>>(ball.GetSphere<1>()[i]) - float_ball[1][i];
		REQUIRE(abs(e) < eps);
	}


}
