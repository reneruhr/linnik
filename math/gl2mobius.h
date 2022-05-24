#pragma once
#include "random_word.h"
#include "arithmetic.h"
#include <array>
#include <cmath>
#include <iostream>
#include <functional>
#include <numeric>
#include <utility>
#include <span>
#include <complex>
#include <vector>

namespace Math
{

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

}
