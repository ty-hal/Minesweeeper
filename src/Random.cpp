#include "Random.h"
#include <ctime>

//Static variables must be redeclared in global space
mt19937 Random::random(time(0));

int Random::Int(int min, int max) {
	uniform_int_distribution<int> dist(min, max);
	return dist(random);
}
float Random::Float(float min, float max) {
	uniform_real_distribution<float> dist(min, max);
	return dist(random);
}
