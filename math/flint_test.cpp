//macos: clear && ~/g++11 -I/opt/local/include  -L/opt/local/lib -lflint -lgmp  flint_test.cpp external_libs/catch2/*.cpp -o flinttest -std=c++20 -Wno-psabi && ./flinttest
#define CATCH_CONFIG_MAIN
#include "external_libs/catch2/catch_amalgamated.hpp"

#include "primes1-78500.h"
#include "random_word.h"
#include "binaryforms.h"

#include "flint/fmpzxx.h"
#include "flint/ulong_extras.h"
#include "flint/arithxx.h"

#include <iostream>
#include <limits>
#include <numeric>

using namespace flint;
using Flint = fmpzxx;
using namespace Math;

TEST_CASE("Integers", "[Flint]")
{
    //std::cout << 2*COEFF_MAX << "  " << COEFF_MIN;
    auto i_max = std::numeric_limits<int>::max(); 
    auto l_max = std::numeric_limits<long>::max(); 
    Flint i_max_(i_max); 
    Flint l_max_(l_max); 

    auto a = l_max / i_max;
    Flint a_(a);
    auto a__ = l_max_ / i_max_;
   
    auto d = l_max / i_max;
    auto r = l_max % i_max;
    
    auto d_ = l_max_ / i_max_;
    auto r_ = ( (l_max_+20)  % i_max_) - 20;
    Flint ten(10);
        
    REQUIRE( a__ == a_ );
    REQUIRE( std::gcd(l_max, i_max) == gcd(l_max_, i_max_) ); 
    REQUIRE( l_max_ + l_max_ == (l_max_+l_max_).evaluate() );
    REQUIRE( d_ == d );
    REQUIRE( r_ == r );
    REQUIRE( (l_max_ * l_max_).is_square() == true );
    REQUIRE( Flint(0).is_zero());
    REQUIRE( Flint(1).is_one());
    REQUIRE( Flint(2).is_even());
    REQUIRE( Flint(3).is_odd());
    REQUIRE( Flint(1000033).is_probabprime());
    REQUIRE( Flint(-1000033).abs() == 1000033);
    REQUIRE( ten+1 >> 1 << 1 == ten);
    REQUIRE(  pow(ten,3u) == 1000);
    REQUIRE( sqrt(l_max_ * l_max_+1) == l_max_);
    REQUIRE( jacobi(Flint(17),Flint(16)) == 1);
}


TEST_CASE("Binaryforms, [Flint]")
{
	auto rg = RandomSequenceGenerator(1337,1000);
  std::vector<Flint> d;
  auto primes = Primes::Get();
   while(d.size() < 10) 
   {
    auto index = rg.Get(5);
    std::sort(begin(index), end(index));
    index.erase(std::unique(begin(index), end(index)), end(index));
    Flint D(1);
    for(auto i : index)
       D *= primes[i];
    if((D-1)%Flint(4)==0) 
        d.emplace_back(D);
   }

   for(auto& D : d)
       std::cout << D << ",\n";
  

   auto split_primes = std::vector<int>{};
   for(auto D : d) 
       for(auto p : primes.subspan(1))
           if(jacobi(D,Flint(p))==1) {
            split_primes.emplace_back(p);
            break;
           }
   std::copy(split_primes.begin(), split_primes.end(),
          std::ostream_iterator<int>(std::cout, "\n"));
    
    
}
