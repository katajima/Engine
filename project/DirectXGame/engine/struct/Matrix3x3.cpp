#include "Matrix3x3.h"

Matrix3x3 Multiply(Matrix3x3 m1, Matrix3x3 m2)
{
	Matrix3x3 temp{};

	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
			for (int w = 0; w < 3; w++) {

				temp.m[y][x] += m1.m[y][w] * m2.m[w][x];

			}
		}
	}

	return temp;
}

Matrix3x3 MakeTranslateMatrix(Vector2 translate)
{
	Matrix3x3 temp{};
	Vector2 result{};

	temp.m[0][0] = 1;
	temp.m[0][1] = 0;
	temp.m[0][2] = 0;
	temp.m[1][0] = 0;
	temp.m[1][1] = 1;
	temp.m[1][2] = 0;
	temp.m[2][2] = 1;
	temp.m[2][0] = translate.x;
	temp.m[2][1] = translate.y;


	return temp;
}

Vector2 Transforms(Vector2 vector, Matrix3x3 matrix)
{
	Vector2 result{};

	result.x = vector.x * matrix.m[0][0] + vector.y * matrix.m[1][0] + 1.0f * matrix.m[2][0];
	result.y = vector.x * matrix.m[0][1] + vector.y * matrix.m[1][1] + 1.0f * matrix.m[2][1];
	float w = vector.x * matrix.m[0][2] + vector.y * matrix.m[1][2] + 1.0f * matrix.m[2][2];
	assert(w != 0.0f);
	result.x /= w;
	result.y /= w;

	return result;
};

Matrix3x3 MakeRotateMatrix(float theta) {
	Matrix3x3 temp{};

	temp.m[0][0] = cosf(theta);
	temp.m[0][1] = sinf(theta);
	temp.m[0][2] = 0;
	temp.m[1][0] = -sinf(theta);
	temp.m[1][1] = cosf(theta);
	temp.m[1][2] = 0;
	temp.m[2][0] = 0;
	temp.m[2][1] = 0;
	temp.m[2][2] = 1;


	return temp;
}
// 拡縮行列
Matrix3x3 MakeScaleMatrix(Vector2 scale) {
	Matrix3x3 temp{};

	temp.m[0][0] = scale.x;
	temp.m[0][1] = 0;
	temp.m[0][2] = 0;
	temp.m[1][0] = 0;
	temp.m[1][1] = scale.y;
	temp.m[1][2] = 0;
	temp.m[2][2] = 1;
	temp.m[2][0] = 0;
	temp.m[2][1] = 0;

	return temp;
}
//正射影行列作成
Matrix3x3 MakeOrthographicMatrix(float left, float top, float right, float bottom) {
	Matrix3x3 temp{};

	temp.m[0][0] = (2.0f / (right - left));
	temp.m[0][1] = 0;
	temp.m[0][2] = 0;

	temp.m[1][0] = 0;
	temp.m[1][1] = (2.0f / (top - bottom));
	temp.m[1][2] = 0;

	temp.m[2][0] = ((left + right) / (left - right));
	temp.m[2][1] = ((top + bottom) / (bottom - top));
	temp.m[2][2] = 1;

	return temp;
};

//ビューポート変換作成
Matrix3x3 MakeViewportMatrix(float left, float top, float width, float height) {
	Matrix3x3 temp{};

	temp.m[0][0] = (width / 2);
	temp.m[0][1] = 0;
	temp.m[0][2] = 0;

	temp.m[1][0] = 0;
	temp.m[1][1] = -(height / 2);
	temp.m[1][2] = 0;
	temp.m[2][0] = left + (width / 2);
	temp.m[2][1] = top + (height / 2);
	temp.m[2][2] = 1;

	return temp;
};
// 逆
Matrix3x3 Inverse(Matrix3x3 matrix) {
	Matrix3x3 temp{};
	float temp2;

	temp2 = 1 / ((matrix.m[0][0] * matrix.m[1][1] * matrix.m[2][2]) + (matrix.m[0][1] * matrix.m[1][2] * matrix.m[2][0]) + (matrix.m[0][2] * matrix.m[1][0] * matrix.m[2][1])
		- (matrix.m[0][2] * matrix.m[1][1] * matrix.m[2][0]) - (matrix.m[0][1] * matrix.m[1][0] * matrix.m[2][2]) - (matrix.m[0][0] * matrix.m[1][2] * matrix.m[2][1]));

	//1列
	temp.m[0][0] = temp2 * ((matrix.m[1][1] * matrix.m[2][2]) - (matrix.m[1][2] * matrix.m[2][1]));//
	temp.m[0][1] = temp2 * -((matrix.m[0][1] * matrix.m[2][2]) - (matrix.m[0][2] * matrix.m[2][1]));//
	temp.m[0][2] = temp2 * ((matrix.m[0][1] * matrix.m[1][2]) - (matrix.m[0][2] * matrix.m[1][1]));//
	//2列
	temp.m[1][0] = temp2 * -((matrix.m[1][0] * matrix.m[2][2]) - (matrix.m[1][2] * matrix.m[2][0]));//
	temp.m[1][1] = temp2 * ((matrix.m[0][0] * matrix.m[2][2]) - (matrix.m[0][2] * matrix.m[2][0]));//
	temp.m[1][2] = temp2 * -((matrix.m[0][0] * matrix.m[1][2]) - (matrix.m[0][2] * matrix.m[1][0]));//
	//3列
	temp.m[2][0] = temp2 * ((matrix.m[1][0] * matrix.m[2][1]) - (matrix.m[1][1] * matrix.m[2][0]));//
	temp.m[2][1] = temp2 * -((matrix.m[0][0] * matrix.m[2][1]) - (matrix.m[0][1] * matrix.m[2][0]));//
	temp.m[2][2] = temp2 * ((matrix.m[0][0] * matrix.m[1][1]) - (matrix.m[0][1] * matrix.m[1][0]));//

	return temp;
};
//アフィン変換
Matrix3x3 MakeAffineMatrix(Vector2 scale, float theta, Vector2 translate) {

	return Multiply(Multiply(MakeScaleMatrix(scale), MakeRotateMatrix(theta)), MakeTranslateMatrix(translate));
}
// 単位行列
Matrix3x3 MakeIdentity3x3()
{
	Matrix3x3 result{};


	for (int x = 0; x < 3; x++) {
		for (int y = 0; y < 3; y++) {
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

Matrix3x3 Matrix3x3::operator*(const Matrix3x3& mat)
{
	return Multiply(*this,mat);
}
