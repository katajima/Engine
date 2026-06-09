#include "Random.h"

uint32_t Random::RandomUint32_t(uint32_t min, uint32_t max)
{
	// スレッドごとに乱数エンジンを持ち、呼び出し間で状態を継続する
	static thread_local std::mt19937 generator(std::random_device{}());
	// 指定範囲内の整数を均等な確率で生成する
	std::uniform_int_distribution<uint32_t> distribution(min, max);
	return distribution(generator);
}

int32_t Random::RandomInt32_t(int32_t min, int32_t max)
{
	// 符号付き整数用の分布で、min〜maxの範囲から生成する
	static thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<int32_t> distribution(min, max);
	return distribution(generator);
}

size_t Random::RandomSize_t(size_t min, size_t max)
{
	// 配列添字やサイズ用途に使うsize_t乱数を生成する
	static thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_int_distribution<size_t> distribution(min, max);
	return distribution(generator);
}

float Random::RandomFloat(float min, float max)
{
	// 引数の大小が逆でも使えるように範囲を補正する
	if (min > max) std::swap(min, max);
	static thread_local std::mt19937 generator(std::random_device{}());
	// 小数は連続一様分布で生成する
	std::uniform_real_distribution<float> distribution(min, max);
	return distribution(generator);
}


double Random::RandomDouble(double min, double max)
{
	// double精度が必要な計算用に、float版とは別の分布で生成する
	static thread_local std::mt19937 generator(std::random_device{}());
	std::uniform_real_distribution<double> distribution(min, max);
	return distribution(generator);
}


Vector2 Random::RandomVector2(float min, float max)
{
	// 各成分を同じ範囲の乱数で生成する
	return { RandomFloat(min,max), RandomFloat(min,max) };
}

Vector3 Random::RandomVector3(float min, float max)
{
	// 3D座標や速度用にXYZを同じ範囲から生成する
	return { RandomFloat(min,max) , RandomFloat(min,max) , RandomFloat(min,max) };
}

Vector4 Random::RandomVector4(float min, float max)
{
	// 色や4次元パラメータ用にXYZWを同じ範囲から生成する
	return { RandomFloat(min,max) , RandomFloat(min,max) , RandomFloat(min,max) , RandomFloat(min,max) };
}

Vector2 Random::RandomVector2(Vector2 min, Vector2 max)
{
	// 成分ごとに異なる最小・最大値を使う
	return { RandomFloat(min.x,max.x), RandomFloat(min.y,max.y) };
}

Vector3 Random::RandomVector3(Vector3 min, Vector3 max)
{
	// XYZそれぞれの範囲指定に従って生成する
	return { RandomFloat(min.x,max.x) , RandomFloat(min.y,max.y) , RandomFloat(min.z,max.z) };
}

Vector4 Random::RandomVector4(Vector4 min, Vector4 max)
{
	// XYZWそれぞれの範囲指定に従って生成する
	return { RandomFloat(min.x,max.x) , RandomFloat(min.y,max.y) , RandomFloat(min.z,max.z) , RandomFloat(min.w,max.w) };
}

bool Random::RandomBool()
{
	// true/falseを同じ確率で返す
	static thread_local std::mt19937 generator(std::random_device{}());
	std::bernoulli_distribution distribution(0.5); // 50%の確率でtrueまたはfalse
	return distribution(generator);
}


float Random::RandFloat(const Range <float>& vec3, std::mt19937& randomEngine)
{
	// 呼び出し元が管理する乱数エンジンを使い、再現性のある乱数を生成する
	std::uniform_real_distribution<float> vec(vec3.min, vec3.max);

	return vec(randomEngine);
}

Vector2 Random::RandVector2(const Range <Vector2>& vec3, std::mt19937& randomEngine)
{
	// Rangeの各成分を個別の分布として扱う
	std::uniform_real_distribution<float> vecX(vec3.min.x, vec3.max.x);
	std::uniform_real_distribution<float> vecY(vec3.min.y, vec3.max.y);


	return Vector2{ vecX(randomEngine),vecY(randomEngine) };
}

Vector3 Random::RandVector3(const Range <Vector3>& vec3, std::mt19937& randomEngine)
{
	// 3成分それぞれに範囲指定を反映する
	std::uniform_real_distribution<float> vecX(vec3.min.x, vec3.max.x);
	std::uniform_real_distribution<float> vecY(vec3.min.y, vec3.max.y);
	std::uniform_real_distribution<float> vecZ(vec3.min.z, vec3.max.z);


	return Vector3{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine) };
}

Vector4 Random::RandVector4(const Range <Vector4>& vec3, std::mt19937& randomEngine)
{
	// 4成分それぞれに範囲指定を反映する
	std::uniform_real_distribution<float> vecX(vec3.min.x, vec3.max.x);
	std::uniform_real_distribution<float> vecY(vec3.min.y, vec3.max.y);
	std::uniform_real_distribution<float> vecZ(vec3.min.z, vec3.max.z);
	std::uniform_real_distribution<float> vecW(vec3.min.w, vec3.max.w);


	return Vector4{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine),vecW(randomEngine) };
}

float Random::RandFloat(const MedianWithRange<float>& vec3, std::mt19937& randomEngine)
{
	// 中央値+幅の指定から最小値と最大値を計算して使う
	std::uniform_real_distribution<float> vec(vec3.Min(), vec3.Max());

	return vec(randomEngine);
}

Vector2 Random::RandVector2(const MedianWithRange<Vector2>& vec3, std::mt19937& randomEngine)
{
	// 中央値+幅の指定を成分ごとの範囲へ展開する
	std::uniform_real_distribution<float> vecX(vec3.Min().x, vec3.Max().x);
	std::uniform_real_distribution<float> vecY(vec3.Min().y, vec3.Max().y);

	return Vector2{ vecX(randomEngine),vecY(randomEngine) };
}

Vector3 Random::RandVector3(const MedianWithRange<Vector3>& vec3, std::mt19937& randomEngine)
{
	// XYZそれぞれの中央値+幅から分布を作る
	std::uniform_real_distribution<float> vecX(vec3.Min().x, vec3.Max().x);
	std::uniform_real_distribution<float> vecY(vec3.Min().y, vec3.Max().y);
	std::uniform_real_distribution<float> vecZ(vec3.Min().z, vec3.Max().z);

	return Vector3{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine) };
}

Vector4 Random::RandVector4(const MedianWithRange<Vector4>& vec3, std::mt19937& randomEngine)
{
	// XYZWそれぞれの中央値+幅から分布を作る
	std::uniform_real_distribution<float> vecX(vec3.Min().x, vec3.Max().x);
	std::uniform_real_distribution<float> vecY(vec3.Min().y, vec3.Max().y);
	std::uniform_real_distribution<float> vecZ(vec3.Min().z, vec3.Max().z);
	std::uniform_real_distribution<float> vecW(vec3.Min().w, vec3.Max().w);

	return Vector4{ vecX(randomEngine),vecY(randomEngine),vecZ(randomEngine),vecW(randomEngine) };
}

Vector3 Random::RandomUnitVector3(std::mt19937& engine)
{
	// [-1, 1]の立方体から候補を作り、単位球の内側だけ採用する
	std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

	Vector3 v;
	do {
		v = { dist(engine), dist(engine), dist(engine) };
	} while (Length(v) > 1.0f); // 単位球の内側

	return Normalize(v); // 単位ベクトル
}
