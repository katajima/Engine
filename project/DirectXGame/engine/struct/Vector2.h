#pragma once
#include <cmath>
#include <math.h>
#include <iostream>
#include <algorithm>

// Vector2
struct Vector2 final {
	float x, y;
	static constexpr size_t Dim = 2;
	float& operator[](size_t i) { return i == 0 ? x : y; }
	const float& operator[](size_t i) const { return i == 0 ? x : y; }


	/// <summary>
	/// == 演算子のオーバーロード
	/// </summary>
	bool operator==(const Vector2& other) const {
		return x == other.x && y == other.y;
	}
	/// <summary>
	/// + 演算子のオーバーロード
	/// </summary>
	Vector2 operator+(const Vector2& other) const {
		return Vector2{ x + other.x, y + other.y };
	}
	/// <summary>
	/// - 演算子のオーバーロード
	/// </summary>
	Vector2 operator+(float other) const {
		return Vector2{ x + other, y + other };
	}

	/// <summary>
	/// - 演算子のオーバーロード
	/// </summary>
	Vector2 operator-(const Vector2& other) const {
		return Vector2{ x - other.x, y - other.y };
	}
	/// <summary>
	/// - 演算子のオーバーロード
	/// </summary>
	Vector2 operator-(float other) const {
		return Vector2{ x - other, y - other };
	}

	/// <summary>
	/// * 演算子のオーバーロード
	/// </summary>
	Vector2 operator*(const Vector2& other) const {
		return Vector2{ x * other.x, y * other.y };
	}
	/// <summary>
	/// * 演算子のオーバーロード
	/// </summary>
	Vector2 operator*(float other) const {
		return Vector2{ x * other, y * other };
	}
	/// <summary>
	/// + 演算子のオーバーロード
	/// </summary>
	Vector2& operator+=(const Vector2& other) {

		x += other.x;
		y += other.y;
		return *this;
	}
	/// <summary>
	/// + 演算子のオーバーロード
	/// </summary>
	Vector2& operator-=(const Vector2& other) {

		x -= other.x;
		y -= other.y;
		return *this;
	}


	/// <summary>
	/// 正規化
	/// </summary>
	Vector2 Normalize() const {
		Vector2 result{};
		float length;

		length = sqrtf((x * x) + (y * y));

		if (length != 0.0f) {
			result.x = x / length;
			result.y = y / length;

		};

		return result;
	};
	/// <summary>
	/// 長さ
	/// </summary>
	float Length() const{
		float result;

		result = sqrtf((x * x) + (y * y));

		return result;
	};

	float LengthSq() const {
		return x * x + y * y;
	}

	/// <summary>
	/// 内積
	/// </summary>
	float Dot(const Vector2& other) const {
		float result;

		result = x * other.x + y * other.y;


		return result;
	};



};

/// <summary>
/// 補間
/// </summary>
static Vector2 Lerp(const Vector2& a, const Vector2& b, float t) {
	Vector2 temp;
	t = std::clamp(t, 0.0f, 1.0f);
	temp.x = a.x * (1.0f - t) + b.x * t;
	temp.y = a.y * (1.0f - t) + b.y * t;

	return temp;
}

/// <summary>
/// 最大値
/// </summary>
static Vector2 Max(const Vector2& v1, const Vector2& v2) {
	Vector2 result{};


	result.x = (std::max)(v1.x, v2.x);
	result.y = (std::max)(v1.y, v2.y);

	return result;
}

/// <summary>
/// 最小値
/// </summary>
static Vector2 Min(const Vector2& v1, const Vector2& v2) {
	Vector2 result{};


	result.x = (std::min)(v1.x, v2.x);
	result.y = (std::min)(v1.y, v2.y);

	return result;
}

/// <summary>
/// 正規化
/// </summary>
static Vector2 Normalize(const Vector2& v) {
	Vector2 result{};
	float length;

	length = sqrtf((v.x * v.x) + (v.y * v.y));

	if (length != 0.0f) {
		result.x = v.x / length;
		result.y = v.y / length;
	};

	return result;
};

/// <summary>
/// 加算
/// </summary>
static Vector2 Add(const Vector2& v1, const Vector2& v2) {
	Vector2 result{};

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;

	return result;
};
/// <summary>
/// 減算
/// </summary>
static Vector2 Subtract(const Vector2& v1, const Vector2& v2) {
	Vector2 result{};

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	
	return result;
}
/// <summary>
/// 大きさ
/// </summary>
static float Length(Vector2 a) {
	float length;

	length = sqrtf(a.x * a.x + a.y * a.y);

	return length;
}
/// <summary>
/// 内積
/// </summary>
static float Dot(const Vector2& a, const Vector2& b) {
	return a.x * b.x + a.y * b.y;
}
/// <summary>
/// 回転
/// </summary>
static Vector2 Rotate(const Vector2& v, float angle) {
	float cosA = std::cos(angle);
	float sinA = std::sin(angle);
	return {
		v.x * cosA - v.y * sinA,
		v.x * sinA + v.y * cosA
	};
}