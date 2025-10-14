#pragma once
#include "DirectXGame/engine/struct/Vector4.h"
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/engine/struct/Vector2.h"
#include "DirectXGame/engine/struct/Structs.h"


namespace ConvertUtility
{
	// Vector4 to Vector3
	static Vector3 ToVector3(const Vector4& vec4)
	{
		return Vector3(vec4.x, vec4.y, vec4.z);
	}
	// Vector3 to Vector4
	static Vector4 ToVector4(const Vector3& vec3, float w = 1.0f)
	{
		return Vector4(vec3.x, vec3.y, vec3.z, w);
	}
	// Vector2 to Vector3
	static Vector3 ToVector3(const Vector2& vec2, float z = 0.0f)
	{
		return Vector3(vec2.x, vec2.y, z);
	}
	// Vector3 to Vector2
	static Vector2 ToVector2(const Vector3& vec3)
	{
		return Vector2(vec3.x, vec3.y);
	}
	// Color to Vector4
	static Vector4 ToVector4(const Color& color)
	{
		return Vector4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
	}
	// Vector4 to Color
	static Color ToColor(const Vector4& vec4)
	{
		return Color(
			static_cast<unsigned char>(vec4.x * 255.0f),
			static_cast<unsigned char>(vec4.y * 255.0f),
			static_cast<unsigned char>(vec4.z * 255.0f),
			static_cast<unsigned char>(vec4.w * 255.0f)
		);
	}

	// uint to int
	static int ToInt(const uint32_t& v) {
		return static_cast<int>(v);
	}
	// float to int
	static int ToInt(const float& v) {
		return static_cast<int>(v);
	}
	// bool to int
	static int ToInt(const bool& v) {
		return v ? 1 : 0;
	}

	// int to uint
	static uint32_t ToUint32(const int& v) {
		return static_cast<uint32_t>(v);
	}
	// float to uint
	static uint32_t ToUint32(const float& v) {
		return static_cast<uint32_t>(v);
	}
	// bool to uint
	static uint32_t ToUint32(const bool& v) {
		return v ? 1 : 0;
	}


	// float to int
	static float ToFloat(const int& v) {
		return static_cast<float>(v);
	}
	// uint to float
	static float ToFloat(const uint32_t& v) {
		return static_cast<float>(v);
	}
	
	// int to bool
	static bool ToBool(const int& v) {
		return v != 0;
	}
	// uint to bool
	static bool ToBool(const uint32_t& v) {
		return v != 0;
	}



	// 
	static float TimeToSeconds(int timeInMilliseconds) {
		return static_cast<float>(timeInMilliseconds) / 1000.0f;
	}

	static float FramesToSeconds(int frameCount, float fps = 60.0f) {
		return static_cast<float>(frameCount) / fps;
	}
}
