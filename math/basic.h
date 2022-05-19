#pragma once

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
}
