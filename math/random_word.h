#pragma once
#include <random>

namespace Math
{

class RandomSequenceGenerator
{
	std::random_device dev_;
	std::mt19937 rng_;
	std::uniform_int_distribution<int> dist_;

public: 
	RandomSequenceGenerator(unsigned int seed, int max) : rng_(dev_()), dist_(std::uniform_int_distribution<>(0, max-1))
	{
		rng_.seed(seed);
	}

	auto Get(int length) -> std::vector<int>
	{
		std::vector<int> sequence(length);
		for(int i = 0; i < length; ++i) sequence[i] = dist_(rng_);
		return sequence;
	}
	
};
}
