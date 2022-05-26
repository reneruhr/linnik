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
	
	friend 	std::ostream& operator<<(std::ostream& os, const QuadraticForm<Int,D>& A)
	{
		os << '(' << A.a_ << ", " << A.b_ << ", " << A.c_ << ')';
		return os;
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

template<class Int,int D>
constexpr auto Reduce(QuadraticForm<Int, D> Q) -> QuadraticForm<Int, D>
{
	auto& a = Q.a_;
	auto& b = Q.b_;
	auto& c = Q.c_;

	bool init = true;
	while(true){
		if(not init or  -a < b and b <= a){
			if(a > c){
				b = -b;
				using std::swap;
				swap(a,c);
			}else{
				if((a==c) and (b<0)) b=-b;
				break;	
			}
		}		

		auto q = b / (2*a);
		auto r = b % (2*a);
		if(r > a) { r=r-2*a; q++; }
		c -= (b+r)*q/2;
		b = r;
		if(init) init = false;
	}
	return Q;
}

template<class Int, int D>
constexpr auto Composition(QuadraticForm<Int, D> P, QuadraticForm<Int, D> Q) -> QuadraticForm<Int,D>
{
	Int a,b,c;
	Int y1,y2,x1,x2,u,v,v1,v2,d,d1,r,s,n;
	using std::swap;
	if(P.a_ > Q.a_) swap(P,Q);
	s = (P.b_+Q.b_)/2;
	n = Q.b_-s;
	
	if(Q.a_%P.a_ == 0){
		y1=0; d=P.a_;
	}else{
		auto uvd = ExtendedEuclid(Q.a_, P.a_);
		u = std::get<0>(uvd);
		v = std::get<1>(uvd);
		d = std::get<2>(uvd);
		y1= u;
	}
	if(s%d == 0){
		y2=-1; x2=0; d1=d;
	}else{
		auto uvd1 = ExtendedEuclid(s, d);
		u = std::get<0>(uvd1);
		v = std::get<1>(uvd1);
		d1 = std::get<2>(uvd1);
		x2 = u;	
		y2 = -v;
	}
	
	v1 = P.a_ / d1; v2 = Q.a_ / d1;
	r = (y1*y2*n-x2*Q.c_) % v1;
	b = Q.b_ + 2*v2*r; a = v1*v2;
	c = (Q.c_*d1 + r*(Q.b_+v2*r))/v1;
	return Reduce(decltype(P)(a,b,c));
}

}
