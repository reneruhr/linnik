#pragma once
#include "gl2mobius.h"
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


//
template <class Mat, int p, int r>
constexpr auto HeckeSphere() -> decltype(auto) 
{
	if constexpr(r==0) return std::array<Mat,1>{Mat{}};
	else{
		std::array<Mat, (p+1)*IntPow(p,r-1)> neighbours;	
		int i = 0;
		for(auto&& n : HeckeDown<Mat,p,r>())
			neighbours[i++] = n;
		neighbours[i] = HeckeUpNeighbour<Mat,p,r>();	
		return neighbours;	
	}	
}

template<class G,class C, int p,int r>
constexpr auto HeckeSphereMobius(C z) -> decltype(auto) 
{
	auto gs = HeckeSphere<G,p,r>();
	
	auto nbs = std::array<C, size(gs)>{};	
	for(int i =0; i< static_cast<int>(size(gs)); ++i) nbs[i] = Reduction(Mobius(z,gs[i]));	
	return nbs;
}

template<int p, int... r>
constexpr auto HeckeBallSize_impl(std::integer_sequence<int, r...>) -> int
{
	return ( ((p+1) * IntPow(p,r)) + ... + 1  );
}

template<int p, int r, typename Indices = std::make_integer_sequence<int,r>>
constexpr auto HeckeBallSize() -> int
{
	if constexpr(r==0) return 1;
	else return HeckeBallSize_impl<p>(Indices{});	
}

template<int p, int r>
constexpr auto HeckeSphereSize() -> int
{
	if constexpr(r==0) return 1;
	else return (p+1)*IntPow(p,r-1);
}

template<class C,int N>
constexpr auto AssignSphere(std::span<C> ball_sphere, std::array<C,N> sphere)
{
    auto b = begin(ball_sphere);
    for(auto& s : sphere)
        *(b++) = s;
}

template <class G, class C, int p, int... r>
constexpr auto FillBall_impl(C z, std::span<C> ball, 
	                     std::integer_sequence<int, r...>)
{
	( AssignSphere<C, HeckeSphereSize<p,r+1>() >
		(ball.subspan(HeckeBallSize<p,r>()),
		   HeckeSphereMobius<G,C,p,r+1>(z))
	,...);    
}

template<class G, class C, int p, int r>
constexpr auto FillBall(C z) -> std::array<C, HeckeBallSize<p,r>()>
{
	auto ball = std::array<C, HeckeBallSize<p,r>()>{};
	ball[0] = z;	
	FillBall_impl<G,C,p>(z, std::span{ball}, std::make_integer_sequence<int,r>{});
	return ball;
}

template<class G, class C, int p, int r>
struct HeckeBall
{
  explicit HeckeBall(C z) :  ball_{FillBall<G,C,p,r>(z)}, z_(z)
    { }

  template <int s>
  constexpr auto GetSphere() -> decltype(auto)
    {
      if constexpr(s==0) 
		    return std::span{ball_}.subspan(0, 1);
      else
	return std::span{ball_}.subspan
	  (HeckeBallSize<p,s-1>(), HeckeSphereSize<p,s>());
    }
  std::array<C, HeckeBallSize<p,r>()> ball_;
  C z_;
};

template<class C, class F>
constexpr auto AssignFloatSphere(std::span<F> float_sphere, std::span<C> sphere)
{
  auto b = begin(float_sphere);
  for(auto& s : sphere)
    *(b++) = F{static_cast<float>(s.real()),
      static_cast<float>(s.imag())};
}

template <class C, class F,int p, class Ball, int... r>
constexpr void FillFloatBall_impl(Ball& ball,
				  std::vector<std::vector<F>>& float_ball,
				  std::integer_sequence<int, r...>)
{
  (( float_ball[r] = std::vector<F>(HeckeSphereSize<p,r>())
     , AssignFloatSphere<C, F>
     (std::span{float_ball[r]}, ball.template GetSphere<r>() ) )
   , ...); 
}

template <class C, int p, int r, class Ball>
auto HeckeBallFloatConverter(Ball& ball)
{
  using F = std::complex<float>;
  std::vector< std::vector<F> > spheres(r+1);
	
  FillFloatBall_impl<C,F,p>(ball, spheres, std::make_integer_sequence<int,r+1>{});
  return spheres;	
}
}
