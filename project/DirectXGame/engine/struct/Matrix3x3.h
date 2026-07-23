#pragma once
#include "Vector2.h"
#include <cmath>
#include <math.h>
#include<assert.h>

//行列
struct Matrix3x3
{
	float m[3][3];

	Matrix3x3 operator*(const Matrix3x3& mat);

	/// <summary>
	/// 単位行列
	/// </summary>
	static Matrix3x3 Identity() {
		return Matrix3x3{ 1,0,0,0,1,0,0,0,1};
	}
	/// <summary>
	/// ワールドポジション取得
	/// </summary>
	Vector2 GetWorldPosition() const {
		// ワールド座標を入れる
		Vector2 worldPos{};
		worldPos.x = m[2][0];
		worldPos.y = m[2][1];
		return worldPos;
	};

};
/// <summary>
/// 行列の積
/// </summary>
Matrix3x3 Multiply(Matrix3x3 m1, Matrix3x3 m2);
/// <summary>
/// 平行行列作成
/// </summary>
Matrix3x3 MakeTranslateMatrix(Vector2 translate);
/// <summary>
/// 行列をVectorに変換
/// </summary>
Vector2 Transforms(Vector2 vector, Matrix3x3 matrix);
/// <summary>
/// 回転行列作成
/// </summary>
Matrix3x3 MakeRotateMatrix(float theta);
/// <summary>
/// 拡縮行列作成
/// </summary>
Matrix3x3 MakeScaleMatrix(Vector2 scale);
/// <summary>
/// 正射影行列作成
/// </summary>
Matrix3x3 MakeOrthographicMatrix(float left, float top, float right, float bottom);
/// <summary>
/// ビューポート変換
/// </summary>
Matrix3x3 MakeViewportMatrix(float left, float top, float width, float height);
/// <summary>
/// 逆行列
/// </summary>
Matrix3x3 Inverse(Matrix3x3 matrix);
/// <summary>
/// アフィン変換
/// </summary>
Matrix3x3 MakeAffineMatrix(Vector2 scale, float theta, Vector2 translate);
/// <summary>
/// 単位行列
/// </summary>
Matrix3x3 MakeIdentity3x3();



////行列の積
//Matrix3x3 Multipy(Matrix3x3 matrix1, Matrix3x3 matrix2);
////正射影行列作成
//Matrix3x3 MakeOrthographicMatrix(float left, float top, float right, float bottom);
////ビューポート変換作成
//Matrix3x3 MakeViewportMatrix(float left, float top, float width, float height);
////アフィン変換
//Matrix3x3 MakeAffineMatrix(Vector2 scale, float theta, Vector2 translate);
////逆
//Matrix3x3 Inverse(Matrix3x3 matrix);
////行列をVectorに変換
//Vector2 Transform(Vector2 vector, Matrix3x3 matrix);

