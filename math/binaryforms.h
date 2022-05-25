#pragma once
#include "primes1-78500.h"
#include <array>
#include "arithmetic.h"
namespace Math{

template<class Int, int D>
struct QuadraticForm
{
	Int a_,b_,c_;
	
	constexpr QuadraticForm() : a_(1), b_(0), c_(Abs(D)) {}
	constexpr QuadraticForm(Int a, Int b, Int c) : a_(a), b_(b), c_(c) {}

	constexpr friend auto operator==(const QuadraticForm<Int,D>& A, const QuadraticForm<Int,D>& B)
	{
		return (A.a_ == B.a_) and (A.b_ == B.b_) and (A.c_ == B.c_ ); 
	} 
	
};

constexpr bool IsPrime(int p)
{
	return	end(Primes::Get()) != std::find(begin(Primes::Get()), end(Primes::Get()), p);
}

constexpr bool IsPrimeAndFundamental(int D)
{
	return  IsPrime(Abs(D)) && ( (D-1)%4 == 0 );
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

template<class Int, int D>
constexpr bool IsReduced(QuadraticForm<Int,D> q){
	return (Abs(q.b_) <= q.a_) and (q.a_ <= q.c_) and
	       ( (Abs(q.b_) == q.a_) or ( q.a_ == q.c_ ) ? q.b_>=0 : true );
}

// Cohen 5.3.5
// todo: change to constexpr vector when available
// for now use tuple first first member being the size, aka class number h(D)
template<class Int, int D>
constexpr auto MakeReducedForms() -> std::tuple<int, std::array<QuadraticForm<Int,D>,Abs(D)>>
{
	static_assert(IsPrimeAndFundamental(D));
	std::array<QuadraticForm<Int,D>, Abs(D)> forms{};
	int h{0};
	Int b{1};
	int  B(SquareRoot(Abs(D)/3));
	Int ac,a,c;
	while(true){
		ac = (b * b - D ) / 4; 
		a = b;
		while(true){
			if( a > 0) {
				if(ac%a == 0){
					c = ac/a;	
					forms[h] = QuadraticForm<Int,D>(a,b,c);
					h++;
					if( (a==b) or (a == c) or (b == 0)) ;
					else{
						forms[h] = QuadraticForm<Int,D>(a,-b,c);
						h++;
					}
				}
			} else a=1;
			a++; 
			if(a * a > ac) break;
		}
		b+=2; 
		if(b > B) break;
	};
	return {h,forms};
}

}
