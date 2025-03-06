#pragma once
#include <cmath>
#include <iostream>
#include <algorithm>
#include <limits>
#include "Vector2.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>

struct Vector3 final {
	float x, y, z;
	
	// 配列のようにアクセスできるオーバーロード
	float& operator[](int index) {
		return (&x)[index]; // x のアドレスを使って配列のようにアクセス
	}

	const float& operator[](int index) const {
		return (&x)[index]; // const バージョン
	}
	
	Vector2 xy() { return Vector2{ x,y }; }




	// == 演算子のオーバーロード
	bool operator==(const Vector3& other) const {
		return x == other.x && y == other.y && z == other.z;
	}
	// = 演算子のオーバーロード
	Vector3 operator=(float other)  {
		return { x = other , y = other , z = other };
	}

	// + 演算子のオーバーロード
	Vector3 operator+(const Vector3& other) const {
		return Vector3{ x + other.x, y + other.y, z + other.z };
	}
	// + 演算子のオーバーロード (スカラーとベクトル) 
	Vector3 operator+(float other) const {
		return Vector3{ x + other, y + other, z + other };;
	}
	// + 演算子のオーバーロード
	Vector3& operator+=(const Vector3& other)  {
		
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	// + 演算子のオーバーロード
	Vector3& operator-=(const Vector3& other)  {
		
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	// - 演算子のオーバーロード
	Vector3 operator-(const Vector3& other) const {
		return Vector3{ x - other.x, y - other.y, z - other.z };
	}
	// - 演算子のオーバーロード
	Vector3 operator-(float other) const {
		return Vector3{ x - other, y - other, z - other };
	}

	// * 演算子のオーバーロード
	Vector3 operator*(const Vector3& other) const {
		return Vector3{ x * other.x, y * other.y, z * other.z };
	}

	// * 演算子のオーバーロード (スカラーとベクトル) 
	Vector3 operator*(float other) const {
		return Vector3{ x * other, y * other, z * other};;
	}
	 

	Vector3 operator-() const {
		return Vector3{ -x,-y,-z};
	};
	 


	// / 演算子のオーバーロード (スカラー除算) 
	Vector3 operator/(float s) const { 
		return Vector3{(x / s), (y / s), (z / s)}; 
	}

	// 正規化
	Vector3 Normalize() const{
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
	// 内積
	float Dot(const Vector3& other) const {
		float result;

		result = x * other.x + y * other.y + z * other.z;


		return result;
	};
	// 長さ
	float Length() {
		float result;

		result = sqrtf((x * x) + (y * y) + (z * z));

		return result;
	};
	float LengthSq() const {
		return x * x + y * y + z * z;
	}
	// ベクトルの要素ごとの絶対値
	Vector3 Abs() const {
		return { std::abs(x), std::abs(y), std::abs(z) };
	}

	// ベクトルのクロス積（外積）
	Vector3 Cross(const Vector3& other) const {
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}

	float Distance(const Vector3& other) const {
		float dx = x - other.x;
		float dy = y - other.y;
		float dz = z - other.z;
		return std::sqrt(dx * dx + dy * dy + dz * dz);
	}
	float DistanceXZ(const Vector3& other) const{
		float dx = x - other.x;
		float dz = z - other.z;
		return std::sqrt(dx * dx + dz * dz);
	}
};


// 補間
static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
	Vector3 temp;
	t = std::clamp(t, 0.0f, 1.0f);
	temp.x = a.x * (1.0f - t) + b.x * t;
	temp.y = a.y * (1.0f - t) + b.y * t;
	temp.z = a.z * (1.0f - t) + b.z * t;

	return temp;
}
// 正規化
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
// 外積
static Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;

	return result;
};
// 内積
static float Dot(const Vector3& v1, const Vector3& v2) {
	float result;

	result = v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;


	return result;
};
// 最大
static Vector3 Max(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};


	result.x = (std::max)(v1.x, v2.x);
	result.y = (std::max)(v1.y, v2.y);
	result.z = (std::max)(v1.z, v2.z);

	return result;
}
// 最小
static Vector3 Min(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};


	result.x = (std::min)(v1.x, v2.x);
	result.y = (std::min)(v1.y, v2.y);
	result.z = (std::min)(v1.z, v2.z);

	return result;
}
// 加算
static Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;


	return result;
};
// 減算
static Vector3 Subtract(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;


	return result;
}

// Vector3同士
static Vector3 Multiply(const Vector3& v1, const Vector3& v2) {
	Vector3 result{};

	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;

	return result;
}
// floatとVector3
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

// 長さ
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

// 2つの点間の距離を計算する関数
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

// 垂直
static Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y,vector.x,0.0f };
	}
	return { 0.0f,-vector.z,vector.y };
}

// Degree変換
static Vector3 RadiansToDegrees(Vector3 radians) {
	Vector3 resurt;

	resurt.x = float(radians.x * (180.0 / (float)M_PI));
	resurt.y = float(radians.y * (180.0 / (float)M_PI));
	resurt.z = float(radians.z * (180.0 / (float)M_PI));

	return resurt;
}

// radian変換
static Vector3 DegreesToRadians(Vector3 degrees) {
	Vector3 resurt;

	resurt.x = float(degrees.x * ((float)M_PI / 180.0));
	resurt.y = float(degrees.y * ((float)M_PI / 180.0));
	resurt.z = float(degrees.z * ((float)M_PI / 180.0));

	return resurt;
}