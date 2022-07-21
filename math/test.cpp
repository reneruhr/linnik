//macos: clear && ~/g++11 test.cpp external_libs/catch2/*.cpp -o mathtest -std=c++20 -Wno-psabi && ./mathtest

#define CATCH_CONFIG_MAIN
#include "external_libs/catch2/catch_amalgamated.hpp"
#include "../math.h"
#include "../tools/tools.h"
#include <sstream>
#include <algorithm>
#include <vector>
#include <complex>
#include <limits>
#include <iomanip>

using namespace std::literals::complex_literals;
using namespace Math;

TEST_CASE("Find multiple Orbits", "[BinaryForm]")
{
  std::cout << "Multiple Orbits\n";
  constexpr int n{50};
  constexpr int D{100000};
  auto [discs, pts ,ps , necklaces] = NecklaceCollection<n,D>();
  for(int i{0}; i<n; ++i){
    auto D = discs[i].first;
    auto h = discs[i].second;
    auto girth = size(necklaces[i]);
    if(h!=girth)
    std::cout << "D = " << D << ". p = " << ps[i] << ". h = " << h << ". girth = " << girth << ".\n";
    }
}


TEST_CASE("Multiple Ds", "[BinaryForm]")
{
  std::cout << "Multiple D's\n";
  constexpr auto Ds = DiscriminantCollection<20>(1000000);
  Tools::for_range<0,20>([&Ds]<auto i>() {
      constexpr auto h = std::get<0>(MakeReducedForms<long, Ds[i], false>());
      std::cout << Ds[i]<< ", " << h <<"\n";
    });
}




TEST_CASE("PartitionForms", "[BinaryForm]")
{
	constexpr auto D = SmallestFundamentalPrimeAbove(50000);
	constexpr auto p = SmallestSplitPrime(D);	
	
	using QF = QuadraticForm<long,D>; 
	
	auto sized_cm_points = MakeReducedForms<long, D>();	
	auto orbit_sizes = PartitionForms<decltype(sized_cm_points), long,D,p>(sized_cm_points);	

	for(auto b = begin(std::get<1>(orbit_sizes));  b<std::get<0>(orbit_sizes)+begin(std::get<1>(orbit_sizes)); ++b)
		std::cout << *b << ", "; 
}

TEST_CASE("Necklace", "[BinaryForm]")
{
	constexpr auto D = SmallestFundamentalPrimeAbove(50000);
	REQUIRE( IsPrimeAndFundamental(D) == true );
	constexpr auto p = SmallestSplitPrime(D);	
	
	using QF = QuadraticForm<long,D>; 
	
	constexpr auto sized_cm_points = MakeReducedForms<long, D>();	
	constexpr auto h = std::get<0>(sized_cm_points);	
	constexpr auto pts = std::get<1>(sized_cm_points);
	
	std::cout << "Found Fundamental D=" << D << " with split prime p=" << p
		  << " of class number h(D)=" << h << ":\n";
	// for(auto&& f : std::span(pts).subspan(0,h))
	//	std::cout << f << "\n ";

	constexpr auto sized_necklace = Necklace<long, D, p>(pts[0]);	
	auto b = begin(std::get<1>(sized_necklace));
	auto girth = std::get<0>(sized_necklace);
	auto e = b+girth;
	auto c = b;
	while( c != e)
		REQUIRE(std::find(begin(pts), end(pts), *c++) != end(pts) );
		/*{
		if(std::find(begin(pts), end(pts), *c++) == end(pts) )
			std::cout << *(c-1) << " not found.\n"
			<< "Reduced: " << IsReduced(*(c-1)) << '\n'
			<< "MakeReduce: " << Reduce(*(c-1)) << '\n';
		}*/

	std::cout  << "Necklace of size " << girth << " at the point "
		  << *c << " has orbit: \n"; 
	while(c != b)
		std::cout << *--c << ", ";
	std::cout << '\n';
}

TEST_CASE("log", "[Arithmetic]")
{
	REQUIRE( Abs(Log(5*5,5)-2.) < 0.00001 );
}

TEST_CASE("PrimeFactorIdeal", "[BinaryForms])")
{
	constexpr auto D{-11};
	constexpr auto p{5};
	using QF = QuadraticForm<int,D>; 
	REQUIRE( Kronecker(D,p)==1 );
	constexpr auto f = Reduce(MakePositive(QF(-15,17,-5)));
	constexpr auto P = Reduce(PrimeFactorIdeal<int,D>(p));
	REQUIRE( f == P );
}

TEST_CASE("Squareroot of D mod 4p", "[Binaryforms]")
{
	using namespace Math;
	{	
	constexpr auto D{-71};
	constexpr auto p = SmallestSplitPrime(D);
	constexpr auto b = SquareRootDMod4p(D,p);
	REQUIRE ((b*b - D)%(4*p) == 0);
	}
	{	
	constexpr auto D{-23};
	constexpr auto p = SmallestSplitPrime(D);
	constexpr auto b = SquareRootDMod4p(D,p);
	REQUIRE ((b*b - D)%(4*p) == 0);
	}
}

TEST_CASE("Compose", "[BinaryForms]")
{
	using namespace Math;
	using QF = QuadraticForm<long,-71>; 
	REQUIRE( IsPrimeAndFundamental(-71) == true);
	QF A{4,3,5};
	QF B{3,1,6};
	QF C{2,1,9};
	QF AB = Composition(A,B);
	REQUIRE( AB == C );
}

TEST_CASE("Reduce", "[BinaryForms]")
{
	using namespace Math;
	using QF = QuadraticForm<long,-23>; 	
	auto S = [](auto Q) { return decltype(Q)(Q.c_, -Q.b_, Q.a_); };
	auto T = [](auto Q) { return decltype(Q)(Q.a_, Q.b_-2*Q.a_, Q.c_+Q.a_-Q.b_); };
	auto r = QF(1,1,6);
	auto q = S(r);
	auto q2 = S(T(T(T(T(S(T(T(S(r)))))))));
	auto p = Reduce(q);
	auto p2 = Reduce(q2);

	REQUIRE( p == r);
	REQUIRE( p2 == r);
}


TEST_CASE("ClassNumber from Terras", "[BinaryForms]")
{
   // Terras p208
  constexpr auto n{12};
	  constexpr std::array<int,n> Ds = {-1'000'003, -1'000'007, -1'000'011,  	
	    -1'000'015, -1'000'019, -1'000'023, -1'000'027,
	    -1'000'031, -1'000'036, -1'000'039, -1'000'043, -1'000'047};
	  constexpr std::array<int,n> Hs = {105, 630, 368, 430, 342, 706, 168,928,192,877,192,
	    508};

//These fail:
// -1'000'020, 320
// -1'000'024, 274
// -1'000'040, 688
	  Tools::for_range<0,n>([&Ds, &Hs]<auto i> {
	      constexpr auto h = std::get<0>(MakeReducedForms<long, Ds[i], false>());
	      constexpr auto h_expected = Hs[i];
	      //std::cout << Ds[i] << " : " << h << " == " << h_expected << '\n';
	  REQUIRE( h == h_expected );
	    });
}

TEST_CASE("ClassNumber", "[BinaryForms]")
{
	using namespace Math;
	{
	using Q = QuadraticForm<long,-23>; 	
	constexpr auto h_forms =  MakeReducedForms<long, -23>();
	constexpr auto h = std::get<0>(h_forms);
	constexpr auto forms = std::get<1>(h_forms);

	REQUIRE( h==3 );
	REQUIRE( std::find(begin(forms),begin(forms)+3, Q(1,1,6)) != begin(forms)+3 );
	}	
	{
	constexpr auto D = -47;
	using Q = QuadraticForm<long,D>; 	
	constexpr auto h = std::get<0>(MakeReducedForms<long, D>());
	REQUIRE( h==5 );
	}
	{
	constexpr auto D = -251;		
	using Q = QuadraticForm<long,D>; 	
	constexpr auto h = std::get<0>(MakeReducedForms<long, D>());
	REQUIRE( h==7 );
	
	constexpr auto forms  = std::get<1>(MakeReducedForms<long, D>());
	auto n{0};
	for( auto&&Q : forms) { 
		REQUIRE( IsReduced(Q) == true );
		n++; if(n==h) break;
	}
	}
	{
	constexpr auto D = -1000003;		
	using Q = QuadraticForm<long,D>; 	
	constexpr auto h = std::get<0>(MakeReducedForms<long, D>());
	REQUIRE( h==105 );
	}
	{
	constexpr auto D = -1000039;		
	using Q = QuadraticForm<long,D>; 	
	constexpr auto h = std::get<0>(MakeReducedForms<long, D>());
	REQUIRE( h==877 );
	}
	{
	constexpr auto D = -1000099;		
	using Q = QuadraticForm<long,D>; 	
	constexpr auto h = std::get<0>(MakeReducedForms<long, D>());
	REQUIRE( h==187 );
	}
}

TEST_CASE("IsPrimitive", "[BinaryForms]")
{
	using namespace Math;
	using Q = QuadraticForm<long,-23>; 	
	constexpr auto primitive = IsPrimitive(Q(1,1,6));	
	constexpr auto reduced = IsReduced(Q(2,1,3));	
	
	REQUIRE( primitive == true );	
	REQUIRE( reduced == true );	
}

TEST_CASE("Primes and Fundamental", "[BinaryForms]")
{
	using namespace Math;
	constexpr auto res1 = IsPrime(1063);
	REQUIRE(  res1 == true );
	constexpr auto res2 = IsPrimeAndFundamental(-5923);
	REQUIRE( res2 == true );
}

TEST_CASE("FastGCD", "[Arithmetic]")
{
using namespace Math;

	REQUIRE( Gcd(11,5) == FastGcd(11,5)  );
	REQUIRE( Gcd(11*3,11) == FastGcd(11*3,11) );
	REQUIRE( Gcd(3*5,3*5*7) == FastGcd(3*5,3*5*7)  );
}

TEST_CASE("GCD", "[Arithmetic]")
{
using namespace Math;

	constexpr auto res = Gcd(12,6);
	REQUIRE( res == 6 );
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
	constexpr auto res = tester(81,5);
	REQUIRE( res == true ); 
	REQUIRE( tester(101,11) == true ); 
	REQUIRE( tester(23424,101) == true ); 
}

TEST_CASE("Integer power mod p", "[Arithmetic]"){
using namespace Math;
	constexpr auto res =ModPow(91,3,5);
	REQUIRE ( (91*91*91 ) % 5 == res );
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
	constexpr auto y = SquareRoot(12345*12345);
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
	auto zs = std::vector<std::complex<double>>{ {1.,1.}, {-0.5,1.}, {0.5,2.}, {-3., 0.5} , {1.1,1.}, {-1.1,1.}, {0.8,0.8}};
	auto zs_red = std::vector<std::complex<double>>{ {0.,1.}, {0.5,1.}, {0.5,2.}, {-0., 0.5}, {0.1,1.}, {-0.1,1.} , {-0.2,.8}};
	for(int i = 0; i<std::size(zs); ++i){
	  // REQUIRE ( ReductionTranslation(zs[i]) == zs_red[i] ) ;
	  //std::cout << zs[i] << " == " << zs_red[i] << '\n';
	  REQUIRE ( ApproximativeEqual(ReductionTranslation(zs[i]), zs_red[i]) ) ;
	}
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
	  auto g = RandomWord<Gamma,2>(25, {u,w});
		std:: cout << g;
		std::cout << z << " ->  ";
		z = Reduction(z);
		std::cout << z << " ->  ";
		z = Mobius(z, g);
		std::cout << z << " ->  ";
		z = Reduction(z);
		std::cout << z << '\n';
	};		
	std::for_each(begin(zs_moved),end(zs_moved), move_z);

	for(std::size_t i = 0; i< size(zs); ++i){
	  //std::cout << zs[i] << " == " << zs_moved[i] << '\n';
	  REQUIRE(  ApproximativeEqual(zs[i], zs_moved[i]));
	}
}

TEST_CASE("CM Points Cast", "[BinaryForms]"){
 using Q = QuadraticForm<long,-23>; 	
	using C = std::complex<float>;
	constexpr auto q = Q(1,1,6);	
	auto z = C{-0.5f, std::sqrt(23.f)/2.f};
	REQUIRE( std::abs(z-ToPoint<Q,C>(q)) <0.0001f );
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
	for(std::size_t i = 0; i < size(ball.GetSphere<1>()); ++i) {
		auto e = 
	static_cast<std::complex<float>>(ball.GetSphere<1>()[i]) - float_ball[1][i];
		REQUIRE(abs(e) < eps);
	}
}

TEST_CASE("Single CM Points", "[BinaryForm]")
{
  auto  pts = CMPoints<-23>();
  std::cout << "CM Points single orbit\n";
  for(auto& p : pts) std::cout << p << '\n';
}

TEST_CASE("Multiple CM Points", "[BinaryForm]")
{
  std::cout << "CM Points Multiple orbit\n";
  constexpr auto n{20};
  auto pts  = std::vector<std::vector<std::complex<float>>>(n);
  constexpr auto Ds = DiscriminantCollection<n>(100);
  Tools::for_range<0,n>([&Ds, &pts]<auto i>() {
      auto arr = CMPoints<Ds[i]>();
      std::cout << Ds[i]<< ", " << size(arr) <<"\n";
      pts[i] = std::vector<std::complex<float>>(size(arr));
      for(auto j{0}; j< size(arr); j++)
	pts[i][j] = arr[j];
    });

  CMPointsCollection<10,100>();
}

TEST_CASE("Multiple Necklaces", "[BinaryForm]")
{
  std::cout << "Multiple Necklaces\n";
  {
  constexpr int n{5};
  auto [discs, pts ,ps , necklaces] = NecklaceCollection<n,1000>();
  for(int i{0}; i<n; ++i){
    std::cout << "D = " << discs[i].first << ". p = " << ps[i] << ". h = " << discs[i].second << ". girth = " << size(necklaces[i]) << ".\n";
    }
  }
  {
  constexpr int n{5};
  auto [discs, pts ,ps , necklaces] = NecklaceCollection<n,100000>();
  for(int i{0}; i<n; ++i){
    std::cout << "D = " << discs[i].first << ". p = " << ps[i] << ". h = " << discs[i].second << ". girth = " << size(necklaces[i]) << ".\n";
    }
  }
/*
  {
  constexpr int n{5};
  auto [discs, pts ,ps , necklaces] = NecklaceCollection<n,200000>();
  for(int i{0}; i<n; ++i){
    std::cout << "D = " << discs[i].first << ". p = " << ps[i] << ". h = " << discs[i].second << ". girth = " << size(necklaces[i]) << ".\n";
    }
  }
*/
}
