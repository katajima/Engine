#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>
#include "Vector2.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include <xmmintrin.h>

// Vector3
struct Vector3 final {
	float x, y, z;

	static constexpr size_t Dim = 3;
	float& operator[](size_t i) { return i == 0 ? x : (i == 1 ? y : z); }
	const float& operator[](size_t i) const { return i == 0 ? x : (i == 1 ? y : z); }

	/// <summary>
	/// 配列のようにアクセスできるオーバーロード
	/// </summary>
	float& operator[](int index) {
		return (&x)[index]; // x のアドレスを使って配列のようにアクセス
	}

	const float& operator[](int index) const {
		return (&x)[index]; // const バージョン
	}
	/// <summary>
	/// xy
	/// </summary>
	Vector2 xy() { return Vector2{ x,y }; }
	/// <summary>
	/// xz
	/// </summary>
	Vector2 xz() { return Vector2{ x,z }; }




	/// <summary>
	/// == 演算子のオーバーロード
	/// </summary>
	bool operator==(const Vector3& other) const {
		const float epsilon = 1e-6f;
		return fabs(x - other.x) < epsilon &&
			fabs(y - other.y) < epsilon &&
			fabs(z - other.z) < epsilon;
	}
	/// <summary>
	/// >= 演算子オーバーロード
	/// </summary>
	bool operator>=(const Vector3& other) const {
		return (x >= other.x) && (y >= other.y) && (z >= other.z);
	}
	/// <summary>
	/// <= 演算子オーバーロード
	/// </summary>
	bool operator<=(const Vector3& other) const {
		return (x <= other.x) && (y <= other.y) && (z <= other.z);
	}

	/// <summary>
	/// = 演算子のオーバーロード
	/// </summary>
	Vector3& operator=(float other) {
		x = other;
		y = other;
		z = other;
		return *this;
	}

	/// <summary>
	/// + 演算子のオーバーロード
	/// </summary>
	Vector3 operator+(const Vector3& other) const {
		return Vector3{ x + other.x, y + other.y, z + other.z };
	}
	/// <summary>
	/// + 演算子のオーバーロード (スカラーとベクトル)
	/// </summary>
	Vector3 operator+(float other) const {
		return Vector3{ x + other, y + other, z + other };;
	}
	/// <summary>
	/// + 演算子のオーバーロード
	/// </summary>
	Vector3& operator+=(const Vector3& other) {

		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	/// <summary>
	/// + 演算子のオーバーロード
	/// </summary>
	Vector3& operator-=(const Vector3& other) {

		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	/// <summary>
	/// * 演算子のオーバーロード
	/// </summary>
	Vector3& operator*=(const Vector3& other) {

		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	/// <summary>
	/// * 演算子のオーバーロード
	/// </summary>
	Vector3& operator*=(const float other) {

		x *= other;
		y *= other;
		z *= other;
		return *this;
	}

	/// <summary>
	/// * 演算子のオーバーロード
	/// </summary>
	Vector3& operator/=(const Vector3& other) {

		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	/// <summary>
	/// * 演算子のオーバーロード
	/// </summary>
	Vector3& operator/=(const float other) {

		x /= other;
		y /= other;
		z /= other;
		return *this;
	}

	/// <summary>
	/// - 演算子のオーバーロード
	/// </summary>
	Vector3 operator-(const Vector3& other) const {
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}
	/// <summary>
	/// - 演算子のオーバーロード
	/// </summary>
	Vector3 operator-(float other) const {
		return Vector3{ x - other, y - other, z - other };
	}

	/// <summary>
	/// * 演算子のオーバーロード
	/// </summary>
	Vector3 operator*(const Vector3& other) const {
		return Vector3{ x * other.x, y * other.y, z * other.z };
	}

	/// <summary>
	/// * 演算子のオーバーロード (スカラーとベクトル)
	/// </summary>
	Vector3 operator*(float other) const {
		return Vector3{ x * other, y * other, z * other };;
	}



	Vector3 operator-() const {
		return Vector3{ -x,-y,-z };
	};



	/// <summary>
	/// / 演算子のオーバーロード (スカラー除算)
	/// </summary>
	Vector3 operator/(float s) const {
		return Vector3{ (x / s), (y / s), (z / s) };
	}
	Vector3 operator/(Vector3 s) const {
		return Vector3{ (x / s.x), (y / s.y), (z / s.z) };
	}

	/// <summary>
	/// 正規化
	/// </summary>
	Vector3 Normalize() const {
		Vector3 result{};
		float length;

		length = sqrtf((x * x) + (y * y) + (z * z));

		if (length != 0.0f) {
			result.x = x / length;
			result.y = y / length;
			result.z = z / length;
		};

		return result;
	};
	/// <summary>
	/// 内積
	/// </summary>
	float Dot(const Vector3& other) const {
		float result;

		result = x * other.x + y * other.y + z * other.z;


		return result;
	};
	/// <summary>
	/// 長さ
	/// </summary>
	float Length() const {
		float result;

		result = sqrtf((x * x) + (y * y) + (z * z));

		return result;
	};
	/// <summary>
	/// 長さ
	/// </summary>
	float LengthSq() const {
		return x * x + y * y + z * z;
	}
	/// <summary>
	/// ベクトルの要素ごとの絶対値
	/// </summary>
	Vector3 Abs() const {
		return { std::abs(x), std::abs(y), std::abs(z) };
	}

	/// <summary>
	/// ベクトルのクロス積（外積）
	/// </summary>
	Vector3 Cross(const Vector3& other) const {
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}
	/// <summary>
	/// 距離
	/// </summary>
	float Distance(const Vector3& other) const {
		float dx = x - other.x;
		float dy = y - other.y;
		float dz = z - other.z;
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}
	/// <summary>
	/// 距離XZ
	/// </summary>
	float DistanceXZ(const Vector3& other) const {
		float dx = x - other.x;
		float dz = z - other.z;
		return std::sqrt(dx * dx + dz * dz);
	}
	// クランプ
	static Vector3 Clamp(Vector3 value, Vector3 min, Vector3 max)
	{
		Vector3 result;
		result.x = (std::max)(min.x, (std::min)(max.x, value.x));
		result.y = (std::max)(min.y, (std::min)(max.y, value.y));
		result.z = (std::max)(min.z, (std::min)(max.z, value.z));
		return result;
	}

	/// <summary>
	/// 補間
	/// </summary>
	static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
		Vector3 temp;
		t = std::clamp(t, 0.0f, 1.0f);
		temp.x = a.x * (1.0f - t) + b.x * t;
		temp.y = a.y * (1.0f - t) + b.y * t;
		temp.z = a.z * (1.0f - t) + b.z * t;

		return temp;
	}

	/// <summary>
	/// 値設定
	/// </summary>
	static Vector3 Set(float v) {
		return { v,v,v };
	}

};


/// <summary>
/// 補間
/// </summary>
static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
	Vector3 temp;
	t = std::clamp(t, 0.0f, 1.0f);
	temp.x = a.x * (1.0f - t) + b.x * t;
	temp.y = a.y * (1.0f - t) + b.y * t;
	temp.z = a.z * (1.0f - t) + b.z * t;

	return temp;
}
/// <summary>
/// 正規化
/// </summary>
static Vector3 Normalize(const Vector3& v) {
	Vector3 result{};
	float length;

	length = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

	if (length != 0.0f) {
		result.x = v.x / length;
		result.y = v.y / length;
		result.z = v.z / length;
	};

	return result;
};
/// <summary>
/// 外積
/// </summary>
static Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
};
/// <summary>
/// 内積
/// </summary>
static float Dot(const Vector3& v1, const Vector3& v2) {
	float result;

	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;


	return result;
};
/// <summary>
/// 最大
/// </summary>
static Vector3 Max(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};


	result.x = (std::max)(v1.x, v2.x);
	result.y = (std::max)(v1.y, v2.y);
	result.z = (std::max)(v1.z, v2.z);

	return result;
}
/// <summary>
/// 最小
/// </summary>
static Vector3 Min(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};


	result.x = (std::min)(v1.x, v2.x);
	result.y = (std::min)(v1.y, v2.y);
	result.z = (std::min)(v1.z, v2.z);

	return result;
}
/// <summary>
/// 加算
/// </summary>
static Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;


	return result;
};
/// <summary>
/// 減算
/// </summary>
static Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;


	return result;
}

/// <summary>
/// Vector3同士
/// </summary>
static Vector3 Multiply(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;

	return result;
}
/// <summary>
/// floatとVector3
/// </summary>
static Vector3 Multiply(const float& v, const Vector3& v1) {
	Vector3 result{};

	result.x = v1.x * v;
	result.y = v1.y * v;
	result.z = v1.z * v;

	return result;
}

// Vector3とfloat
static Vector3 Multiply(const Vector3& v1, const float& v2)
{
	Vector3 result{};

	result.x = v1.x * v2;
	result.y = v1.y * v2;
	result.z = v1.z * v2;

	return result;
}

/// <summary>
/// 長さ
/// </summary>
static float Length(const Vector3& v) {
	float result;

	result = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

	return result;
};

static float LengthSquared(const Vector3& v) {
	return v.x * v.x + v.y * v.y + v.z * v.z;
}

// 距離
static float Distance(const Vector3& point1, const Vector3& point2)
{
	float dx = point1.x - point2.x;
	float dy = point1.y - point2.y;
	float dz = point1.z - point2.z;
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

/// <summary>
/// 2つの点間の距離を計算する関数
/// </summary>
static float DistanceXZ(const Vector3& point1, const Vector3& point2) {
	float dx = point1.x - point2.x;
	float dz = point1.z - point2.z;
	return std::sqrt(dx * dx + dz * dz);
}

// 正射影ベクトル
static Vector3 Project(const Vector3& v1, const Vector3& v2)
{
	Vector3 result{};
	float v2length = Dot(v2, v2);

	float dot = Dot(v1, v2);


	result.x = dot / v2length * v2.x;
	result.y = dot / v2length * v2.y;
	result.z = dot / v2length * v2.z;


	return result;
}

/// <summary>
/// 垂直
/// </summary>
static Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

/// <summary>
/// absDot
/// </summary>
static float AbsDot(const Vector3& a, const Vector3& b) {
	return std::abs(Dot(a, b));
}
