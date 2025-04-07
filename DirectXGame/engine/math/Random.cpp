#include "Random.h"
#include <random>

uint32_t Random::RandomUint32_t(uint32_t min, uint32_t max)
{
	static thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<uint32_t> distribution(min, max);
	return distribution(generator);
}

int32_t Random::RandomInt32_t(int32_t min, int32_t max)
{
	static thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<int32_t> distribution(min, max);
	return distribution(generator);
}

size_t Random::RandomSize_t(size_t min, size_t max)
{
	static thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<size_t> distribution(min, max);
	return distribution(generator);
}

float Random::RandomFloat(float min, float max)
{
	static thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(generator);
}


double Random::RandomDouble(double min, double max)
{
	static thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_real_distribution<double> distribution(min, max);
	return distribution(generator);
}


Vector2 Random::RandomVector2(float min, float max)
{
	return { RandomFloat(min,max), RandomFloat(min,max)};
}

Vector3 Random::RandomVector3(float min, float max)
{
	return { RandomFloat(min,max) , RandomFloat(min,max) , RandomFloat(min,max) };
}

Vector4 Random::RandomVector4(float min, float max)
{
	return { RandomFloat(min,max) , RandomFloat(min,max) , RandomFloat(min,max) , RandomFloat(min,max) };
}

bool Random::RandomBool()
{
	static thread_local std::mt19937 generator(std::random_device{}());
	std::bernoulli_distribution distribution(0.5); // 50%の確率でtrueまたはfalse
	return distribution(generator);
}
