#pragma once
#include <optional>
namespace Math
{


template <class Int>
constexpr Int PosMod(Int a, Int b)
{
    return (a % b + b) % b;
}

constexpr double Log(double x, double base) 
{ // using gcc built-in constexpr
	return std::log(x)/std::log(base);
}

constexpr double SquareRoot(double x)
{
	return std::pow(x, 0.5);
} 

template<class Int>
constexpr Int Abs(Int x)
{
	return x > 0 ? x : -x;
} 

template<class T>
constexpr int IntFloor(T a)
{
	auto i = static_cast<int>(a);
	return i<=a ? i : i-1;
}

template<class T>
constexpr int IntCeil(T a)
{
	auto i = static_cast<int>(a);
	return i>=a ? i : i+1;
}

template<class T>
constexpr int IntNearest(T a)
{
  return IntFloor(a+0.5);
}

template<class T>
constexpr int IntNearestTo0(T a)
{
  return static_cast<int>(a);
}

constexpr int IntPow(int b, int e, int res = 1)
{
	return e==0 ? res : IntPow(b, e-1, b*res); 
}

// Cohen 1.2.1
template <class Int>
constexpr Int FastPow(Int b, int e)
{
	auto y = Int(1);
	auto z = b;
	while(true){
		if(e&1) y*=z;
		if(e==0) return y;
		e/= 2;
		z*=z;	
	}
}

// Cohen 1.2.1
template <class Int>
constexpr Int ModPow(Int b, int e, int p)
{
	auto y = Int(1);
	auto z = b % p;
	while(true){
		if(e&1) y*=z;
		if(e==0) return y;
		e/= 2;
		z*=z;	
		z%=p;
	}
}

// Cohen 1.3.6
template <class Int>
constexpr std::tuple<Int, Int, Int> ExtendedEuclid(Int a, Int b)
{
	Int u{1},d{a},v{0};
	if(b==0) return {u,v,d};
	Int v1{0},v3{b},t1,t3, q;
	while(true){
		if(v3==0){ 
			v = (d-a*u)/b;
			return {u,v,d};
		}
		q = d/v3; t3 = d%v3;
		t1 = u-q*v1;
		u = v1;
		d = v3;
		v1 = t1;
		v3 = t3;
	}
}

template<class Int>
constexpr Int FastGcd(Int a, Int b)
{
	while(b != 0) {
		a = a%b;
		using std::swap;
		swap(a,b);	
	}
	return a;
} 

template<class Int>
constexpr Int Gcd(Int a, Int b)
{
	auto  d = std::get<2>(ExtendedEuclid(a,b));
	return d;
} 

template<class Int, class... Ints>
constexpr Int Gcd(Int a, Int b, Ints... rest)
{
	auto  e = Gcd(b, rest...);
	auto  d = std::get<2>(ExtendedEuclid(a,e));
	return d;
} 
// Cohen 1.4.10
template <class Int>
constexpr int Kronecker(Int a, Int b)
{	
	if(b==0) return abs(a)==1 ? 1 : 0;
	if( not (a&1) and not (b&1) ) return 0;
	Int v{0};
	const auto Remove2 = [](Int& x, int& v) { 
				while(not (x&1) ){ v++; x/=2; }};
	Remove2(b,v);
	const std::array<int,8> table{0,1,0,-1,0,-1,0,1};
	int k{1}; 
	if(v&1) k=table[a&7];
	if(b<0) { b=-b; if(a<0) k=-k; }
	
	while(true){
		if(a==0) return b>1 ? 0 : k;
		v=0;
		Remove2(a,v);
		if(v&1) k = k * table[b&7];
		if(a&b&2) k=-k; 
		auto r = abs(a);
		a = b % r;
		b = r;
	}
}

//Cohen 1.5.1
template <class Int>
constexpr std::optional<int> SquareRootMod(Int a, int p)
{
	int e =0;
	int q = p-1;
	auto Remove2 = [](Int& x, int& e) {
		 while(not (x&1) ) { x/=2; e++; } };
	Remove2(q, e);
	int n = 1;
	while( Kronecker(n,p) != -1 ) n++;
	auto z = ModPow(n,q,p);
	auto y{z};
	auto r{e};
	auto x{ModPow(a, (q-1)/2,p)};
	auto b{ ( a*x*x ) % p};
	x = ( a * x ) % p;	
	while(true){
		if( (b-1) % p == 0 ) return x;	
		int m=1;
		for(;m<r; ++m) if(ModPow(b, FastPow(2,m),p)==1) break;
		if(m==r) return {};
		auto t = ModPow(y,FastPow(2,r-m-1),p);
		y = ( t * t ) % p;
		r = m % p;
		x = ( x * t ) % p;
		b = ( b * y ) % p;	
	}
}

}
