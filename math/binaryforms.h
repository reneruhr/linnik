#pragma once
#include "primes1-78500.h"

namespace Math{

template<class Int, int D>
class QuadraticForm
{
	Int a_,b_,c_;
	
	constexpr QuadraticForm(Int a, Int b, Int c) : a_(a), b_(b), c_(c) {}
};


constexpr bool IsPrime(int p)
{
	return	end(Primes::Get()) != std::find(begin(Primes::Get()), end(Primes::Get()), p);
}


constexpr bool IsPrimeAndFundamental(int D)
{
	return  IsPrime(abs(D)) && ( (D-1)%4 == 0 );
}

template <class Int, int D>
constexpr bool Valid(Int a, Int b, Int c)
{
	return b*b-4*a*c == D;
}

template<class Int, int D>
constexpr bool IsPrimitive(QuadraticForm<Int,D> q){
	return Gcd(q.a_,q.b_,q.c_) == 1;
}

}
