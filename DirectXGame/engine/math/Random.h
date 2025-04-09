#pragma once

#include <cstdint>
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector4.h"

class Random
{
public:

	// uint32_t型の乱数
	static uint32_t RandomUint32_t(uint32_t min, uint32_t max);
	// int32_t型の乱数
	static int32_t RandomInt32_t(int32_t min, int32_t max);
	// int32_t型の乱数
	static size_t RandomSize_t(size_t min, size_t max);
	// float型の乱数
	static float RandomFloat(float min, float max);
	// double型の乱数
	static double RandomDouble(double min, double max);
	// Vector2型の乱数
	static Vector2 RandomVector2(float min, float max);
	// Vector3型の乱数
	static Vector3 RandomVector3(float min, float max);
	// Vector4型の乱数
	static Vector4 RandomVector4(float min, float max);
	// bool型の乱数
	static bool RandomBool();

};


