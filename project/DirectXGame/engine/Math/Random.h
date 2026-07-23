#pragma once

#include <cstdint>
#include <random>
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Structs.h"

/// <summary>
/// ランダムクラス
/// </summary>
class Random
{
public:

	/// <summary>
	/// uint32_t型の乱数
	/// </summary>
	static uint32_t RandomUint32_t(uint32_t min, uint32_t max);
	/// <summary>
	/// int32_t型の乱数
	/// </summary>
	static int32_t RandomInt32_t(int32_t min, int32_t max);
	/// <summary>
	/// int32_t型の乱数
	/// </summary>
	static size_t RandomSize_t(size_t min, size_t max);
	/// <summary>
	/// float型の乱数
	/// </summary>
	static float RandomFloat(float min, float max);
	/// <summary>
	/// double型の乱数
	/// </summary>
	static double RandomDouble(double min, double max);
	/// <summary>
	/// Vector2型の乱数
	/// </summary>
	static Vector2 RandomVector2(float min, float max);
	/// <summary>
	/// Vector3型の乱数
	/// </summary>
	static Vector3 RandomVector3(float min, float max);
	/// <summary>
	/// Vector4型の乱数
	/// </summary>
	static Vector4 RandomVector4(float min, float max);
	/// <summary>
	/// Vector2型の乱数
	/// </summary>
	static Vector2 RandomVector2(Vector2 min, Vector2 max);
	/// <summary>
	/// Vector3型の乱数
	/// </summary>
	static Vector3 RandomVector3(Vector3 min, Vector3 max);
	/// <summary>
	/// Vector4型の乱数
	/// </summary>
	static Vector4 RandomVector4(Vector4 min, Vector4 max);
	/// <summary>
	/// bool型の乱数
	/// </summary>
	static bool RandomBool();


	/// <summary>
	/// ランダムエンジンを使った乱数生成(float)
	/// </summary>
	static float RandFloat(const Range <float>& vec3, std::mt19937& randomEngine);

	/// <summary>
	/// ランダムエンジンを使った乱数生成(Vector2)
	/// </summary>
	static Vector2 RandVector2(const Range <Vector2>& vec3, std::mt19937& randomEngine);

	/// <summary>
	/// ランダムエンジンを使った乱数生成(Vector3)
	/// </summary>
	static Vector3 RandVector3(const Range <Vector3>& vec3, std::mt19937& randomEngine);

	/// <summary>
	/// ランダムエンジンを使った乱数生成(Vector4)
	/// </summary>
	static Vector4 RandVector4(const Range <Vector4>& vec3, std::mt19937& randomEngine);


	/// <summary>
	/// ランダムエンジンを使った乱数生成(float)
	/// </summary>
	static float RandFloat(const MedianWithRange <float>& vec3, std::mt19937& randomEngine);

	/// <summary>
	/// ランダムエンジンを使った乱数生成(Vector2)
	/// </summary>
	static Vector2 RandVector2(const MedianWithRange <Vector2>& vec3, std::mt19937& randomEngine);

	/// <summary>
	/// ランダムエンジンを使った乱数生成(Vector3)
	/// </summary>
	static Vector3 RandVector3(const MedianWithRange <Vector3>& vec3, std::mt19937& randomEngine);

	/// <summary>
	/// ランダムエンジンを使った乱数生成(Vector4)
	/// </summary>
	static Vector4 RandVector4(const MedianWithRange <Vector4>& vec3, std::mt19937& randomEngine);

	/// <summary>
	/// ランダムエンジンを使った乱数生成(球)
	/// </summary>
	static Vector3 RandomUnitVector3(std::mt19937& engine);

};


