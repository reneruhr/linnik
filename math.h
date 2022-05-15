#pragma once
#include "math/random_word.h"
#include <array>
#include <cmath>
#include <iostream>
#include <functional>
#include <numeric>

namespace Math
{

//Naive but used for small exponents only
constexpr int IntPow(int b, int e, int res = 1)
{
	return e==0 ? res : IntPow(b, e-1, b*res); 
}

//required since Floor,Ceil from numeric only constexpr from C++23
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

//Column Major
template <class Scalar>
class GL2
{
	std::array<Scalar,4> data_;
public:
	using value_type = Scalar; 
	constexpr GL2() : data_{ Scalar(1), Scalar(0), Scalar(0), Scalar(1) } {}
	constexpr GL2(Scalar a, Scalar c, Scalar b, Scalar d) : data_{ Scalar(a), Scalar(c), Scalar(b), Scalar(d) } {}

// <=> not supported on Mac
/*	constexpr friend auto operator<=>(const GL2<Scalar>& A, const GL2<Scalar>& B)
	{
		return  (A.data_ <=> B.data_ );
	} 
*/	
	constexpr friend auto operator==(const GL2<Scalar>& A, const GL2<Scalar>& B)
	{
		return  (A.data_ == B.data_ );
	} 

	constexpr auto At(int i, int j) const -> Scalar { return data_[i+2*j]; } 
	
	constexpr friend auto operator*(const GL2<Scalar>& A, const GL2<Scalar>& B) -> GL2<Scalar>
	{
		auto res = GL2<Scalar>(  A.At(0,0) * B.At(0,0) + A.At(0,1) * B.At(1,0) ,
					 A.At(1,0) * B.At(0,0) + A.At(1,1) * B.At(1,0) , 
					 A.At(0,0) * B.At(0,1) + A.At(0,1) * B.At(1,1) ,
					 A.At(1,0) * B.At(0,1) + A.At(1,1) * B.At(1,1) 
					);
		return res; 
	}


	constexpr friend auto Transpose(const GL2<Scalar>& g) -> GL2<Scalar>
	{
		return {g.data_[0], g.data_[2], g.data_[1], g.data_[3] };
	}
		
	friend 	std::ostream& operator<<(std::ostream& os, const GL2<Scalar>& A)
	{
		os << A.At(0,0) << ", " << A.At(0,1) << "\n" 
		   << A.At(1,0)  << ", " << A.At(1,1) << "\n";
		return os;
	}
	
	constexpr friend auto a(const GL2<Scalar>& g) -> const Scalar& { return g.data_[0]; }
	constexpr friend auto b(const GL2<Scalar>& g) -> const Scalar& { return g.data_[2]; }
	constexpr friend auto c(const GL2<Scalar>& g) -> const Scalar& { return g.data_[1]; }
	constexpr friend auto d(const GL2<Scalar>& g) -> const Scalar& { return g.data_[3]; }
};



template <class Mat, int p>
struct HeckeNeighbours
{
	using value_type = typename Mat::value_type;
	
	std::array<Mat, p+1> neighbours_;
	constexpr HeckeNeighbours()
	{
		for(int i=0; i<p; ++i)
			neighbours_[i] = Mat(p,0,i,1);
			neighbours_[p] = Mat(1,0,0,p);	
	}
	
	constexpr auto operator[](int i) const -> const Mat& { return neighbours_[i]; }	
};

template <class Mat, int p, int r>
constexpr auto HeckeUpNeighbour() -> Mat
{
	if constexpr(r==0) return Mat{};
	else {
		auto nb = Mat(1,0,0,IntPow(p,r));
		return nb;
	}
}

	template <class Mat, int p, int r>
constexpr auto HeckeDown() -> decltype(auto) 
{
	if constexpr(r==0) return std::array<Mat,0>{};
	else {
		constexpr int size = (p+1)*IntPow(p,r-1)-1;
		std::array<Mat, size> neighbours;
		auto close_neighbours = HeckeNeighbours<Mat,p>{};
		int i = 0;
		if constexpr(r==1)
		{ 
			for(int j = 0; j<p; ++j)
				neighbours[i++] =  close_neighbours[j];
			return neighbours;
		}else{
			auto up = HeckeUpNeighbour<Mat,p,r-1>(); 	
			for(int j = 0; j<p-1; ++j)
				neighbours[i++] =  up * close_neighbours[j+1];
			for(int j = 0; j<p; ++j)
				for(auto&& a : HeckeDown<Mat,p,r-1>())
					neighbours[i++] = a * close_neighbours[j];
			return neighbours;
		}}
}

template <class Mat, int p, int r>
constexpr auto HeckeSphere() -> decltype(auto) 
{
	if constexpr(r==0) return std::array<Mat,0>{};
	else{
		std::array<Mat, (p+1)*IntPow(p,r-1)> neighbours;	
		int i = 0;
		for(auto&& n : HeckeDown<Mat,p,r>())
			neighbours[i++] = n;
			neighbours[i] = HeckeUpNeighbour<Mat,p,r>();	
		return neighbours;	
	}	
}


//Right Action 
//   dz - b
//   ------
//   -cz + a
template<class G, class C>
constexpr auto Mobius(const C& z, const G& g) -> C
{
	auto w = (static_cast<C::value_type>(d(g))*z 
		- static_cast<C::value_type>(b(g))) 
		/ 
		(-static_cast<C::value_type>(c(g))*z 
		+ static_cast<C::value_type>(a(g)) );
	return w;
}

template<class C>
constexpr auto ReductionTranslation(C z) -> C
{
	auto x = real(z);
	if (x > 0.5)
		z-= C(IntCeil(x),0); 
	else if (x <= -0.5)
		z-= C(IntFloor(x),0);
	return z;
}

template<class C>
constexpr auto Reduction(C z) -> C
{
	while(true){
		z = ReductionTranslation(z);
		if(norm(z) < 1. || (norm(z)==1. && real(z) < 0.) )
			z = ReductionTranslation(-1. / z);
		else break;
	}
	return z;	
}

template<class Gamma, int alphabet_length>
auto RandomWord(int length, const std::vector<Gamma>& alphabet) -> Gamma
{
	static auto gen = RandomSequenceGenerator(1u, alphabet_length);
	auto sequence = gen.Get(length);	
	auto sequence_maker = [&alphabet](const Gamma& g, int b)
		{
			auto gb = g * alphabet[b]; 
			return gb;
		};
	auto g = 
	std::accumulate(begin(sequence), end(sequence), Gamma(), sequence_maker);	
	return g;
}


template<class G,class C, int p,int r>
auto HeckeSphereMobius(C z) -> decltype(auto) 
{
	auto gs = HeckeSphere<G,p,r>();
	
	auto nbs = std::array<C, size(gs)>();	
	for(int i =0; i< size(gs); ++i) nbs[i] = Reduction(Mobius(z,gs[i]));	
	return nbs;
}

}
