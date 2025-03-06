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

float* GetMatrix(Matrix4x4 mat);

const float* GetMatrixPointer(const Matrix4x4& mat);

Matrix4x4 DirectionToDirection(const Vector3& from, const Vector3& to);
Vector4 Multiply(const Matrix4x4& m, const Vector4& v);

Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);

Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2);

//行列の積
Matrix4x4 Multiply(const Matrix4x4& v1, const Matrix4x4& v2);

//単位行列
Matrix4x4 MakeIdentity4x4();

// 座標変換Vector3
Vector3 Transforms(const Vector3& vector, const Matrix4x4& matrix);

// 座標変換Vector4
Vector4 Transforms(const Vector4& vec, const Matrix4x4& mat);

// 
Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m);


//移動行列
Matrix4x4 MakeTranslateMatrix(const  Vector3& translate);
//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const  Vector3& scale);
//回転行列X
Matrix4x4 MakeRotateXMatrix(float rotate);
//回転行列Y
Matrix4x4 MakeRotateYMatrix(float rotate);
//回転行列Z
Matrix4x4 MakeRotateZMatrix(float rotate);
//回転行列XYZ
Matrix4x4 MakeRotateXYZ(Vector3 rotate);
//逆行列
Matrix4x4 Inverse(const Matrix4x4& m);
//転置行列
Matrix4x4 Transpose(const Matrix4x4& m);
//アフィン変換
Matrix4x4 MakeAffineMatrix(const  Vector3& scale, const  Vector3& rotate, const  Vector3& translate);
//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
//透視射影行列
Matrix4x4 MakePerspectiveFovMatrix(float forY, float aspectRatio, float nearClip, float farClip);
//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float leht, float top, float width, float height, float minDepth, float maxDepth);
