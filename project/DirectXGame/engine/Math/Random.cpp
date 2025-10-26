#include "Random.h"

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
	if (min > max) std::swap(min, max);
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
	return { RandomFloat(min,max), RandomFloat(min,max) };
}

Vector3 Random::RandomVector3(float min, float max)
{
	return { RandomFloat(min,max) , RandomFloat(min,max) , RandomFloat(min,max) };
}

Vector4 Random::RandomVector4(float min, float max)
{
	return { RandomFloat(min,max) , RandomFloat(min,max) , RandomFloat(min,max) , RandomFloat(min,max) };
}

Vector2 Random::RandomVector2(Vector2 min, Vector2 max)
{
	return { RandomFloat(min.x,max.x), RandomFloat(min.y,max.y) };
}

Vector3 Random::RandomVector3(Vector3 min, Vector3 max)
{
	return { RandomFloat(min.x,max.x) , RandomFloat(min.y,max.y) , RandomFloat(min.z,max.z) };
}

Vector4 Random::RandomVector4(Vector4 min, Vector4 max)
{
	return { RandomFloat(min.x,max.x) , RandomFloat(min.y,max.y) , RandomFloat(min.z,max.z) , RandomFloat(min.w,max.w) };
}

bool Random::RandomBool()
{
	static thread_local std::mt19937 generator(std::random_device{}());
	std::bernoulli_distribution distribution(0.5); // 50%の確率でtrueまたはfalse
	return distribution(generator);
}


float Random::RandFloat(const Range <float>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vec(vec3.min, vec3.max);

	return vec(randomEngine);
}

Vector2 Random::RandVector2(const Range <Vector2>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vecX(vec3.min.x, vec3.max.x);
	std::uniform_real_distribution<float> vecY(vec3.min.y, vec3.max.y);


	return Vector2{ vecX(randomEngine),vecY(randomEngine) };
}

Vector3 Random::RandVector3(const Range <Vector3>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vecX(vec3.min.x, vec3.max.x);
	std::uniform_real_distribution<float> vecY(vec3.min.y, vec3.max.y);
	std::uniform_real_distribution<float> vecZ(vec3.min.z, vec3.max.z);


	return Vector3{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine) };
}

Vector4 Random::RandVector4(const Range <Vector4>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vecX(vec3.min.x, vec3.max.x);
	std::uniform_real_distribution<float> vecY(vec3.min.y, vec3.max.y);
	std::uniform_real_distribution<float> vecZ(vec3.min.z, vec3.max.z);
	std::uniform_real_distribution<float> vecW(vec3.min.w, vec3.max.w);


	return Vector4{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine),vecW(randomEngine) };
}

float Random::RandFloat(const MedianWithRange<float>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vec(vec3.Min(), vec3.Max());

	return vec(randomEngine);
}

Vector2 Random::RandVector2(const MedianWithRange<Vector2>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vecX(vec3.Min().x, vec3.Max().x);
	std::uniform_real_distribution<float> vecY(vec3.Min().y, vec3.Max().y);

	return Vector2{ vecX(randomEngine),vecY(randomEngine) };
}

Vector3 Random::RandVector3(const MedianWithRange<Vector3>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vecX(vec3.Min().x, vec3.Max().x);
	std::uniform_real_distribution<float> vecY(vec3.Min().y, vec3.Max().y);
	std::uniform_real_distribution<float> vecZ(vec3.Min().z, vec3.Max().z);

	return Vector3{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine) };
}

Vector4 Random::RandVector4(const MedianWithRange<Vector4>& vec3, std::mt19937& randomEngine)
{
	std::uniform_real_distribution<float> vecX(vec3.Min().x, vec3.Max().x);
	std::uniform_real_distribution<float> vecY(vec3.Min().y, vec3.Max().y);
	std::uniform_real_distribution<float> vecZ(vec3.Min().z, vec3.Max().z);
	std::uniform_real_distribution<float> vecW(vec3.Min().w, vec3.Max().w);

	return Vector4{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine),vecW(randomEngine) };
}

Vector3 Random::RandomUnitVector3(std::mt19937& engine)
{
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	Vector3 v;
	do {
		v = { dist(engine), dist(engine), dist(engine) };
	} while (Length(v) > 1.0f); // 単位球の内側

	return Normalize(v); // 単位ベクトル
}