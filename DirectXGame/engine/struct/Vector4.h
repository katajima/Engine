#pragma once
#include "Vector3.h"
#include "DirectXGame/engine/Color/Color.h"

struct Vector4 final {
	float x;
	float y;
	float z;
	float w;

	static constexpr size_t Dim = 4;
	float& operator[](size_t i) { return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)); }
	const float& operator[](size_t i) const { return i == 0 ? x : (i == 1 ? y : (i == 2 ? z : w)); }

	Vector3 xyz() { return Vector3{ x,y,z }; }


	// == 演算子のオーバーロード
	bool operator==(const Vector4& other) const {
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	// + 演算子のオーバーロード
	Vector4 operator+(const Vector4& other) const {
		return Vector4{ x + other.x, y + other.y, z + other.z,w + other.w };
	}

	// - 演算子のオーバーロード
	Vector4 operator-(const Vector4& other) const {
		return Vector4{ x - other.x, y - other.y, z - other.z,w - other.w };
	}

	// * 演算子のオーバーロード
	Vector4 operator*(const Vector4& other) const {
		return Vector4{ x * other.x, y * other.y, z * other.z,w * other.w };
	}
	// * 演算子のオーバーロード
	Vector4 operator*(const float& other) const {
		return Vector4{ x * other, y * other, z * other,w * 1 };
	}
	//
	Vector4 operator+(const Vector3& other) const {
		return Vector4{ x + other.x, y + other.y, z + other.z,w };
	}
	//
	Vector4 operator=(const Vector3& other) const {
		return Vector4{ other.x, other.y, other.z,w };
	}

	/*Vector4  operator=(const Color& other) const {
		return Vector4{ other.r, other.g, other.b,other.a};
	}*/

};

//Vector4 
