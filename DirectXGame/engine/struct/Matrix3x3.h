#pragma once
#include "Vector2.h"

//行列
struct Matrix3x3
{
	float m[3][3];
};

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

