//macos: clear && ~/g++11 unit_test.cpp external_libs/catch2/*.cpp -o mathtest -std=c++20 -Wno-psabi && ./mathtest

#define CATCH_CONFIG_MAIN
#include "external_libs/catch2/catch_amalgamated.hpp"
#include "../math.h"
#include <sstream>
#include <algorithm>
#include <vector>
#include <complex>

using namespace std::literals::complex_literals;


TEST_CASE("IntPow", "[Utils]"){
{using namespace Math;
	REQUIRE( IntPow(2,0) == 1 ); 
	REQUIRE( IntPow(2,1) == 2 ); 
	REQUIRE( IntPow(2,2) == 4 ); 
}}

TEST_CASE("Ceil and Floor", "[Utils]"){
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
/*
TEST_CASE("Mobius Reduction", "[Complex]"){
	using namespace Math;
	auto zs = std::vector<std::complex<double>>
		  { {1.,1.} };
*/
