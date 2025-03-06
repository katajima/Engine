#pragma once
#include "Vector3.h"
#include "Vector4.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <math.h>
#include<assert.h>

//行列
struct Matrix4x4
{
public:
	float m[4][4];

	static Matrix4x4 Identity() {
		return Matrix4x4{ 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	}
	// 行列のポインタを取得する関数
	const float* GetMatrixPointer() const {
		return &m[0][0];  // 行列データの最初の要素のポインタを返す
	}

	Matrix4x4 operator*(const Matrix4x4& mat);

    Matrix4x4 MakeRotateAxisAngle(const Vector3& axis, float angle) {
        Matrix4x4 result;

        float c = cos(angle);
        float s = sin(angle);
        float t = 1.0f - c;

        result.m[0][0] = t * axis.x * axis.x + c;
        result.m[0][1] = t * axis.x * axis.y + s * axis.z;
        result.m[0][2] = t * axis.x * axis.z - s * axis.y;
        result.m[0][3] = 0.0f;

        result.m[1][0] = t * axis.x * axis.y - s * axis.z;
        result.m[1][1] = t * axis.y * axis.y + c;
        result.m[1][2] = t * axis.y * axis.z + s * axis.x;
        result.m[1][3] = 0.0f;

        result.m[2][0] = t * axis.x * axis.z + s * axis.y;
        result.m[2][1] = t * axis.y * axis.z - s * axis.x;
        result.m[2][2] = t * axis.z * axis.z + c;
        result.m[2][3] = 0.0f;

        result.m[3][0] = 0.0f;
        result.m[3][1] = 0.0f;
        result.m[3][2] = 0.0f;
        result.m[3][3] = 1.0f;

        return result;
    }






	Vector3 GetWorldPosition() const {
		// ワールド座標を入れる
		Vector3 worldPos{};
		worldPos.x = m[3][0];
		worldPos.y = m[3][1];
		worldPos.z = m[3][2];
		return worldPos;
	};

    Vector3 Transform(Vector3 vec) const {
        return {
            vec.x * m[0][0] + vec.y * m[1][0] + vec.z * m[2][0] + m[3][0], // X成分
            vec.x * m[0][1] + vec.y * m[1][1] + vec.z * m[2][1] + m[3][1], // Y成分
            vec.x * m[0][2] + vec.y * m[1][2] + vec.z * m[2][2] + m[3][2]  // Z成分
        };
    }

private:

};

static float* GetMatrix(Matrix4x4 mat) {
    return &mat.m[0][0];  // 行列の最初の要素のポインタを返す
}
static const float* GetMatrixPointer(const Matrix4x4& mat)
{
    return &mat.m[0][0]; // 行列データの先頭要素のアドレスを返す
}



static Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to) {
    Matrix4x4 result{};
    Vector3 u = Normalize(from); // 正規化された from ベクトル
    Vector3 v = Normalize(to);   // 正規化された to ベクトル

    float cosTheta = Dot(u, v);
    if (cosTheta > 0.9999f) {
        // ほぼ同じ方向の場合、単位行列を返す
        result.Identity();
        return result;
    }
    else if (cosTheta < -0.9999f) {
        // 完全に逆向きの場合、任意の垂直軸を回転軸に設定
        Vector3 orthogonal = (fabs(u.x) > fabs(u.z)) ? Vector3(-u.y, u.x, 0.0f) : Vector3(0.0f, -u.z, u.y);
        orthogonal = Normalize(orthogonal);
        return result.MakeRotateAxisAngle(orthogonal, float(M_PI)); // 180度回転
    }

    Vector3 n = Normalize(Cross(u, v)); // 正規化された回転軸
    float sinTheta = sqrt(1.0f - cosTheta * cosTheta);
    float k = 1.0f - cosTheta;

    result.m[0][0] = cosTheta + n.x * n.x * k;
    result.m[0][1] = n.x * n.y * k + n.z * sinTheta;
    result.m[0][2] = n.x * n.z * k - n.y * sinTheta;
    result.m[0][3] = 0.0f;

    result.m[1][0] = n.x * n.y * k - n.z * sinTheta;
    result.m[1][1] = cosTheta + n.y * n.y * k;
    result.m[1][2] = n.y * n.z * k + n.x * sinTheta;
    result.m[1][3] = 0.0f;

    result.m[2][0] = n.x * n.z * k + n.y * sinTheta;
    result.m[2][1] = n.y * n.z * k - n.x * sinTheta;
    result.m[2][2] = cosTheta + n.z * n.z * k;
    result.m[2][3] = 0.0f;

    result.m[3][0] = 0.0f;
    result.m[3][1] = 0.0f;
    result.m[3][2] = 0.0f;
    result.m[3][3] = 1.0f;

    return result;
}



static Vector4 Multiply(const Matrix4x4& m, const Vector4& v) {
    Vector4 result;
    result.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0] * v.w;
    result.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1] * v.w;
    result.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2] * v.w;
    result.w = m.m[0][3] * v.x + m.m[1][3] * v.y + m.m[2][3] * v.z + m.m[3][3] * v.w;
    return result;
}

static Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2)
{
    Matrix4x4 result{};


    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            result.m[y][x] = m1.m[y][x] + m2.m[y][x];
        }
    }


    return result;
}

static Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2)
{
    Matrix4x4 result{};


    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            result.m[y][x] = m1.m[y][x] - m2.m[y][x];
        }
    }


    return result;
}

//行列の積
static Matrix4x4 Multiply(const Matrix4x4& v1, const Matrix4x4& v2) {
    Matrix4x4 result{};

    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            for (int z = 0; z < 4; z++) {
                float value1 = v1.m[y][z];
                float value2 = v2.m[z][x];

                if (value1 == 0.0f || value2 == 0.0f) {
                    result.m[y][x] += 0.0f; // ゼロを掛け算した結果はゼロ
                }
                else {
                    result.m[y][x] += value1 * value2;
                }
            }
        }
    }
    return result;
};

//単位行列
static Matrix4x4 MakeIdentity4x4() {
    Matrix4x4 result{};


    for (int x = 0; x < 4; x++) {
        for (int y = 0; y < 4; y++) {
            if (x == y) {
                result.m[y][x] = 1.0f;
            }
            else {
                result.m[y][x] = 0.0f;
            }
        }
    }
    return result;
}

// 座標変換Vector3
static Vector3 Transforms(const Vector3& vector, const Matrix4x4& matrix) {
    Vector3 result{};

    result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + vector.z * matrix.m[2][0] + 1.0f * matrix.m[3][0];
    result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + vector.z * matrix.m[2][1] + 1.0f * matrix.m[3][1];
    result.z = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + vector.z * matrix.m[2][2] + 1.0f * matrix.m[3][2];

    float w = vector.x * matrix.m[0][3] + vector.y * matrix.m[1][3] + vector.z * matrix.m[2][3] + 1.0f * matrix.m[3][3];
    assert(w != 0.0f);
    result.x /= w;
    result.y /= w;
    result.z /= w;
    return result;
}

// 座標変換Vector4
static Vector4 Transforms(const Vector4& vec, const Matrix4x4& mat)
{
    return Vector4{
        vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + vec.w * mat.m[3][0],
        vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + vec.w * mat.m[3][1],
        vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + vec.w * mat.m[3][2],
        vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + vec.w * mat.m[3][3]
    };
}

// 
static Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {
    Vector3 result{

        v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
        v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
        v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
    };

    return result;
}