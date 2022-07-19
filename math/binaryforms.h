#pragma once
#include "primes1-100000.h"
#include <array>
#include "arithmetic.h"
#include "../tools/tools.h"

namespace Math{

template<class Int, int D>
struct QuadraticForm
{
	Int a_,b_,c_;
	
	constexpr QuadraticForm() : a_(1), b_(0), c_(Abs(D)) {}
	constexpr QuadraticForm(Int a, Int b, Int c) : a_(a), b_(b), c_(c) {}
	constexpr int Disc() const { return D; }
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

template<class Q, class C>
constexpr auto ToPoint(const Q& q) -> C
{
  auto z = C{static_cast<C::value_type>(-q.b_), static_cast<C::value_type>(SquareRoot(Abs(q.Disc())))};
  z /= static_cast<C::value_type>(2*q.a_);
  return z;
}

constexpr bool IsPrime(int p)
{
	return	end(Primes::Get()) != std::find(begin(Primes::Get()), end(Primes::Get()), p);
}

constexpr bool IsPrimeAndFundamental(int D)
{
	return  IsPrime(Abs(D)) && ( (D-1)%4 == 0 );
}

constexpr int SmallestSplitPrime(int D)
{
	for(auto p : Primes::Get().subspan(1))
		if(Kronecker(D,p)==1) return p;
	return 0;
}

constexpr int SmallestFundamentalPrimeAbove(int absD)
{
	auto D_ptr = std::lower_bound(begin(Primes::Get()),end(Primes::Get()), absD);
	while( (-*D_ptr -1) % 4 != 0) ++D_ptr ;
	return -*D_ptr ;
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
// first member is size, aka class number h(D)
template<class Int, int D, bool IsFundamental = true>
constexpr auto MakeReducedForms() -> std::tuple<int, std::array<QuadraticForm<Int,D>,Abs(D)>>
{
  //static_assert(IsPrimeAndFundamental(D));
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
			  if( (ac%a == 0) and (IsFundamental or (Gcd(a, b, ac/a) == 1))){
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

// Cohen 4.5.2
// Q positive
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

		//auto q = b / (2*a);
		auto r = PosMod(b,2*a);
		auto q = (b-r) / (2*a);
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
	
	v1 = P.a_ / d1; 
	v2 = Q.a_ / d1;
	r = PosMod(y1*y2*n-x2*Q.c_, v1);
	b = Q.b_ + 2*v2*r;
	a = v1*v2;
	c = (Q.c_*d1 + r*(Q.b_+v2*r))/v1;
	return Reduce(decltype(P)(a,b,c));
}



//x^2 = D Mod 4p
// where
// p odd and splits
// D = 1 mod 4 
constexpr auto SquareRootDMod4p(int D, int p) -> int
{
	auto x = *SquareRootMod(D,p);
	if(not (x&1)) x+=p;
	return x;
}

template <class Int, int D>
constexpr auto PrimeFactorIdeal(int p) -> QuadraticForm<Int,D>
{
	auto b = SquareRootDMod4p(D,p);
	return QuadraticForm<Int,D>(p,b, (b*b-D) / (4*p));
}

template <class QF>
constexpr auto MakePositive(QF f) -> QF
{
	if(f.a_ < 0) return QF(-f.a_, f.b_, -f.c_);
	return f;
}

template<class Int, int D, int p>
constexpr auto Necklace(QuadraticForm<Int, D> f) -> 
std::pair<int, std::array<QuadraticForm<Int, D>, Abs(D)>>
{
	auto P = PrimeFactorIdeal<Int,D>(p);
	auto necklace = std::array<QuadraticForm<Int, D>, Abs(D)>{};
	auto b = begin(necklace);
	*b = f;
	while(true) {
		if(auto Q = Composition(*b++, P); Q != f) *b = Q;
		else return {std::distance(begin(necklace), b), necklace};
	} 
}


template<class Int, int D, int p, int orbit_bound, class Map>
constexpr auto Orbit(QuadraticForm<Int, D> f, Map map) -> 
std::pair<int, std::array<QuadraticForm<Int, D>, orbit_bound>>
{
	auto necklace = std::array<QuadraticForm<Int, D>, orbit_bound>{};
	auto b = begin(necklace);
	*b = f;
	while(true) {
		if(auto Q = map(*b++); Q != f) *b = Q;
		else return {std::distance(begin(necklace), b), necklace};
	} 
}
// FormsContainer = std::tuple<int, std::array<QuadraticForm<Int,D>,Abs(D)>>
// returns array with orbit lengths.
template <class FormsContainer, class Int, int D, int p>
constexpr auto PartitionForms(FormsContainer& sized_forms) 
-> std::pair<int, std::array<int, Abs(D)>>
{
	using QF = QuadraticForm<Int,D>;
	auto h = std::get<0>(sized_forms);	
	auto pts = std::get<1>(sized_forms);

	auto b = begin(pts);
	auto e = b+h;

	constexpr auto bound = Abs(D);
	auto P = PrimeFactorIdeal<Int,D>(p);
	auto times_p = [P](const QF& f) { return Composition(f,P); };
	
	auto sizes = std::array<int, Abs(D)>{};
	auto s = begin(sizes);

	while(b != e){
		auto sized_necklace = Orbit<Int, D, p, bound>(*b, times_p);
		auto necklace = std::get<1>(sized_necklace);
		*s++ = std::get<0>(sized_necklace);
		b = std::partition(b,e, 
			[&necklace](const auto& form) { 
			return std::find(begin(necklace),end(necklace), form) 
			       != end(necklace); }); 
	}
	return {std::distance(begin(sizes),s), sizes};	
}


template<int n>
constexpr
std::array<int, n>
DiscriminantCollection(int D)
{
  std::array<int,n> discs_{};
  for(int i{0}; i<n; ++i){
    D = SmallestFundamentalPrimeAbove(std::abs(D)); 
    discs_[i] = D--;
  }
  return discs_;
}

template <int D>
constexpr auto CMPoints()
{
  constexpr auto data = MakeReducedForms<long, D>();	
  constexpr auto h  = std::get<0>(data);
  std::array<std::complex<float>, h> pts;
  constexpr auto q_pts = std::get<1>(data);
  for(auto i{0}; i<h; ++i) 
	pts[i] = ToPoint<decltype(q_pts[i]),std::complex<float>>(q_pts[i]);
  return pts;
}

template <int n, int D>
constexpr auto CMPointsCollection()
{
  using F = std::complex<float>;
  auto discs= std::vector<std::pair<int,int>>(n);
  auto pts  = std::vector<std::vector<F>>(n);
  constexpr auto Ds = DiscriminantCollection<n>(D);
  Tools::for_range<0,n>([&Ds, &pts, &discs]<auto i>() {
      auto arr = CMPoints<Ds[i]>();
      auto h = size(arr);
      discs[i].first = Ds[i];
      discs[i].second = h;
      pts[i] = std::vector<F>(h);
      for(auto j{0}; j< h; j++)
	pts[i][j] = arr[j];
    });
  return std::pair{discs,pts};
}
}
