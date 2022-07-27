#pragma once
#include <random>
using namespace std;

class Random
{
	static mt19937 random;
public:
	//Call Int(min,max) function in any file using Random::Random::Int(0, 10);
	static int Int(int min, int max);
	static float Float(float min, float max);
};

