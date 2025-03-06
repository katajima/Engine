#include"MathFanctions.h"
#include<cmath>
#include"assert.h"

#pragma region Math

float Length(const float& v) {
	float result;

	result = sqrtf((v * v));

	return result;
};

float Clamp(float t) {

	if (1.0f <= t) {
		t = 1.0f;
	}
	if (0.0f >= t) {
		t = 0.0f;
	}

	return t;
}

float Clamp(float t, float min, float max) {

	if (max <= t) {
		t = max;
	}
	if (min >= t) {
		t = min;
	}

	return t;
}

float Clamp3(float value, float min, float max) {
	return (std::max)(min, (std::min)(value, max));
}

Vector3 Bezier(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t) {

	Vector3 p0p1 = Lerp(p0, p1, t);

	Vector3 p1p2 = Lerp(p1, p2, t);

	Vector3 p = Lerp(p0p1, p1p2, t);

	return Lerp(p0p1, p1p2, t);

}


//移動行列
Matrix4x4 MakeTranslateMatrix(const  Vector3& translate) {
	Matrix4x4 result{};

	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;

	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1;

	return result;
}
//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const  Vector3& scale) {
	Matrix4x4 result{};

	result.m[0][0] = scale.x;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = scale.y;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = scale.z;
	result.m[2][3] = 0;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;

	return result;
}
//回転行列X
Matrix4x4 MakeRotateXMatrix(float rotate) {
	Matrix4x4 result{};

	result.m[0][0] = 1;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;
	result.m[1][0] = 0;
	result.m[1][1] = std::cos(rotate);
	result.m[1][2] = std::sin(rotate);
	result.m[1][3] = 0;
	result.m[2][0] = 0;
	result.m[2][1] = -std::sin(rotate);
	result.m[2][2] = std::cos(rotate);
	result.m[2][3] = 0;
	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;


	return result;
}
//回転行列Y
Matrix4x4 MakeRotateYMatrix(float rotate) {
	Matrix4x4 result{};

	result.m[0][0] = std::cos(rotate);
	result.m[0][1] = 0;
	result.m[0][2] = -std::sin(rotate);;
	result.m[0][3] = 0;
	result.m[1][0] = 0;
	result.m[1][1] = 1;
	result.m[1][2] = 0;
	result.m[1][3] = 0;
	result.m[2][0] = std::sin(rotate);
	result.m[2][1] = 0;
	result.m[2][2] = std::cos(rotate);
	result.m[2][3] = 0;
	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;


	return result;
}
//回転行列Z
Matrix4x4 MakeRotateZMatrix(float rotate) {
	Matrix4x4 result{};

	result.m[0][0] = std::cos(rotate);
	result.m[0][1] = std::sin(rotate);
	result.m[0][2] = 0;
	result.m[0][3] = 0;
	result.m[1][0] = -std::sin(rotate);
	result.m[1][1] = std::cos(rotate);
	result.m[1][2] = 0;
	result.m[1][3] = 0;
	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1;
	result.m[2][3] = 0;
	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = 0;
	result.m[3][3] = 1;


	return result;
}
//回転行列XYZ
Matrix4x4 MakeRotateXYZ(Vector3 rotate)
{
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	return Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
}
//逆行列
Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result{};
	float result2;

	result2 =
		1 / ((m.m[0][0] * m.m[1][1] * m.m[2][2] * m.m[3][3])
			+ (m.m[0][0] * m.m[1][2] * m.m[2][3] * m.m[3][1])
			+ (m.m[0][0] * m.m[1][3] * m.m[2][1] * m.m[3][2])

			- (m.m[0][0] * m.m[1][3] * m.m[2][2] * m.m[3][1])
			- (m.m[0][0] * m.m[1][2] * m.m[2][1] * m.m[3][3])
			- (m.m[0][0] * m.m[1][1] * m.m[2][3] * m.m[3][2])

			- (m.m[0][1] * m.m[1][0] * m.m[2][2] * m.m[3][3])
			- (m.m[0][2] * m.m[1][0] * m.m[2][3] * m.m[3][1])
			- (m.m[0][3] * m.m[1][0] * m.m[2][1] * m.m[3][2])

			+ (m.m[0][3] * m.m[1][0] * m.m[2][2] * m.m[3][1])
			+ (m.m[0][2] * m.m[1][0] * m.m[2][1] * m.m[3][3])
			+ (m.m[0][1] * m.m[1][0] * m.m[2][3] * m.m[3][2])

			+ (m.m[0][1] * m.m[1][2] * m.m[2][0] * m.m[3][3])
			+ (m.m[0][2] * m.m[1][3] * m.m[2][0] * m.m[3][1])
			+ (m.m[0][3] * m.m[1][1] * m.m[2][0] * m.m[3][2])

			- (m.m[0][3] * m.m[1][2] * m.m[2][0] * m.m[3][1])
			- (m.m[0][2] * m.m[1][1] * m.m[2][0] * m.m[3][3])
			- (m.m[0][1] * m.m[1][3] * m.m[2][0] * m.m[3][2])

			- (m.m[0][1] * m.m[1][2] * m.m[2][3] * m.m[3][0])
			- (m.m[0][2] * m.m[1][3] * m.m[2][1] * m.m[3][0])
			- (m.m[0][3] * m.m[1][1] * m.m[2][2] * m.m[3][0])


			+ (m.m[0][3] * m.m[1][2] * m.m[2][1] * m.m[3][0])
			+ (m.m[0][2] * m.m[1][1] * m.m[2][3] * m.m[3][0])
			+ (m.m[0][1] * m.m[1][3] * m.m[2][2] * m.m[3][0]));


	//一行目一列目
	result.m[0][0] = (m.m[1][1] * m.m[2][2] * m.m[3][3]) + (m.m[1][2] * m.m[2][3] * m.m[3][1]) + (m.m[1][3] * m.m[2][1] * m.m[3][2])
		- (m.m[1][3] * m.m[2][2] * m.m[3][1]) - (m.m[1][2] * m.m[2][1] * m.m[3][3]) - (m.m[1][1] * m.m[2][3] * m.m[3][2]);
	//一行目二列目
	result.m[0][1] = -(m.m[0][1] * m.m[2][2] * m.m[3][3]) - (m.m[0][2] * m.m[2][3] * m.m[3][1]) - (m.m[0][3] * m.m[2][1] * m.m[3][2])
		+ (m.m[0][3] * m.m[2][2] * m.m[3][1]) + (m.m[0][2] * m.m[2][1] * m.m[3][3]) + (m.m[0][1] * m.m[2][3] * m.m[3][2]);
	//一行目三列目
	result.m[0][2] = (m.m[0][1] * m.m[1][2] * m.m[3][3]) + (m.m[0][2] * m.m[1][3] * m.m[3][1]) + (m.m[0][3] * m.m[1][1] * m.m[3][2])
		- (m.m[0][3] * m.m[1][2] * m.m[3][1]) - (m.m[0][2] * m.m[1][1] * m.m[3][3]) - (m.m[0][1] * m.m[1][3] * m.m[3][2]);
	//一行目四列目
	result.m[0][3] = -(m.m[0][1] * m.m[1][2] * m.m[2][3]) - (m.m[0][2] * m.m[1][3] * m.m[2][1]) - (m.m[0][3] * m.m[1][1] * m.m[2][2])
		+ (m.m[0][3] * m.m[1][2] * m.m[2][1]) + (m.m[0][2] * m.m[1][1] * m.m[2][3]) + (m.m[0][1] * m.m[1][3] * m.m[2][2]);


	//二行目一列目
	result.m[1][0] = -(m.m[1][0] * m.m[2][2] * m.m[3][3]) - (m.m[1][2] * m.m[2][3] * m.m[3][0]) - (m.m[1][3] * m.m[2][0] * m.m[3][2])
		+ (m.m[1][3] * m.m[2][2] * m.m[3][0]) + (m.m[1][2] * m.m[2][0] * m.m[3][3]) + (m.m[1][0] * m.m[2][3] * m.m[3][2]);
	//二行目二列目
	result.m[1][1] = (m.m[0][0] * m.m[2][2] * m.m[3][3]) + (m.m[0][2] * m.m[2][3] * m.m[3][0]) + (m.m[0][3] * m.m[2][0] * m.m[3][2])
		- (m.m[0][3] * m.m[2][2] * m.m[3][0]) - (m.m[0][2] * m.m[2][0] * m.m[3][3]) - (m.m[0][0] * m.m[2][3] * m.m[3][2]);
	//二行目三列目
	result.m[1][2] = -(m.m[0][0] * m.m[1][2] * m.m[3][3]) - (m.m[0][2] * m.m[1][3] * m.m[3][0]) - (m.m[0][3] * m.m[1][0] * m.m[3][2])
		+ (m.m[0][3] * m.m[1][2] * m.m[3][0]) + (m.m[0][2] * m.m[1][0] * m.m[3][3]) + (m.m[0][0] * m.m[1][3] * m.m[3][2]);

	//二行目四列目
	result.m[1][3] = (m.m[0][0] * m.m[1][2] * m.m[2][3]) + (m.m[0][2] * m.m[1][3] * m.m[2][0]) + (m.m[0][3] * m.m[1][0] * m.m[2][2])
		- (m.m[0][3] * m.m[1][2] * m.m[2][0]) - (m.m[0][2] * m.m[1][0] * m.m[2][3]) - (m.m[0][0] * m.m[1][3] * m.m[2][2]);


	//三行目一列目
	result.m[2][0] = (m.m[1][0] * m.m[2][1] * m.m[3][3]) + (m.m[1][1] * m.m[2][3] * m.m[3][0]) + (m.m[1][3] * m.m[2][0] * m.m[3][1])
		- (m.m[1][3] * m.m[2][1] * m.m[3][0]) - (m.m[1][1] * m.m[2][0] * m.m[3][3]) - (m.m[1][0] * m.m[2][3] * m.m[3][1]);
	//三行目二列目
	result.m[2][1] = -(m.m[0][0] * m.m[2][1] * m.m[3][3]) - (m.m[0][1] * m.m[2][3] * m.m[3][0]) - (m.m[0][3] * m.m[2][0] * m.m[3][1])
		+ (m.m[0][3] * m.m[2][1] * m.m[3][0]) + (m.m[0][1] * m.m[2][0] * m.m[3][3]) + (m.m[0][0] * m.m[2][3] * m.m[3][1]);
	//三行目三列目
	result.m[2][2] = (m.m[0][0] * m.m[1][1] * m.m[3][3]) + (m.m[0][1] * m.m[1][3] * m.m[3][0]) + (m.m[0][3] * m.m[1][0] * m.m[3][1])
		- (m.m[0][3] * m.m[1][1] * m.m[3][0]) - (m.m[0][1] * m.m[1][0] * m.m[3][3]) - (m.m[0][0] * m.m[1][3] * m.m[3][1]);
	//三行目四列目
	result.m[2][3] = -(m.m[0][0] * m.m[1][1] * m.m[2][3]) - (m.m[0][1] * m.m[1][3] * m.m[2][0]) - (m.m[0][3] * m.m[1][0] * m.m[2][1])
		+ (m.m[0][3] * m.m[1][1] * m.m[2][0]) + (m.m[0][1] * m.m[1][0] * m.m[2][3]) + (m.m[0][0] * m.m[1][3] * m.m[2][1]);


	//四行目一列目
	result.m[3][0] = -(m.m[1][0] * m.m[2][1] * m.m[3][2]) - (m.m[1][1] * m.m[2][2] * m.m[3][0]) - (m.m[1][2] * m.m[2][0] * m.m[3][1])
		+ (m.m[1][2] * m.m[2][1] * m.m[3][0]) + (m.m[1][1] * m.m[2][0] * m.m[3][2]) + (m.m[1][0] * m.m[2][2] * m.m[3][1]);
	//四行目二列目
	result.m[3][1] = (m.m[0][0] * m.m[2][1] * m.m[3][2]) + (m.m[0][1] * m.m[2][2] * m.m[3][0]) + (m.m[0][2] * m.m[2][0] * m.m[3][1])
		- (m.m[0][2] * m.m[2][1] * m.m[3][0]) - (m.m[0][1] * m.m[2][0] * m.m[3][2]) - (m.m[0][0] * m.m[2][2] * m.m[3][1]);
	//四行目三列目
	result.m[3][2] = -(m.m[0][0] * m.m[1][1] * m.m[3][2]) - (m.m[0][1] * m.m[1][2] * m.m[3][0]) - (m.m[0][2] * m.m[1][0] * m.m[3][1])
		+ (m.m[0][2] * m.m[1][1] * m.m[3][0]) + (m.m[0][1] * m.m[1][0] * m.m[3][2]) + (m.m[0][0] * m.m[1][2] * m.m[3][1]);
	//四行目四列目
	result.m[3][3] = (m.m[0][0] * m.m[1][1] * m.m[2][2]) + (m.m[0][1] * m.m[1][2] * m.m[2][0]) + (m.m[0][2] * m.m[1][0] * m.m[2][1])
		- (m.m[0][2] * m.m[1][1] * m.m[2][0]) - (m.m[0][1] * m.m[1][0] * m.m[2][2]) - (m.m[0][0] * m.m[1][2] * m.m[2][1]);

	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {

			result.m[y][x] = result.m[y][x] * result2;

		}
	}


	return result;
}
//転置行列
Matrix4x4 Transpose(const Matrix4x4& m) {
	Matrix4x4 result{};


	for (int x = 0; x < 4; x++) {
		for (int y = 0; y < 4; y++) {
			result.m[y][x] = m.m[x][y];
		}
	}
	return result;
}
//アフィン変換
Matrix4x4 MakeAffineMatrix(const  Vector3& scale, const  Vector3& rotate, const  Vector3& translate) {
	Matrix4x4 result{};

	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);
	Matrix4x4 rotateXYZMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));


	result.m[0][0] = scale.x * rotateXYZMatrix.m[0][0];
	result.m[0][1] = scale.x * rotateXYZMatrix.m[0][1];
	result.m[0][2] = scale.x * rotateXYZMatrix.m[0][2];
	result.m[0][3] = 0;
	result.m[1][0] = scale.y * rotateXYZMatrix.m[1][0];
	result.m[1][1] = scale.y * rotateXYZMatrix.m[1][1];
	result.m[1][2] = scale.y * rotateXYZMatrix.m[1][2];
	result.m[1][3] = 0;
	result.m[2][0] = scale.z * rotateXYZMatrix.m[2][0];
	result.m[2][1] = scale.z * rotateXYZMatrix.m[2][1];
	result.m[2][2] = scale.z * rotateXYZMatrix.m[2][2];
	result.m[2][3] = 0;
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1;

	//SafeMatrix(result);

	return result;
};

Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& rotate, const Vector3& translate) {
	// 結果行列の初期化
	Matrix4x4 result{};
	result.Identity();
	// 回転行列を生成（回転が正規化済みであることを想定）
	Matrix4x4 rotationMatrix = rotate.MakeRotateMatrix();

	// スケールを適用
	result.m[0][0] = scale.x * rotationMatrix.m[0][0];
	result.m[0][1] = scale.x * rotationMatrix.m[0][1];
	result.m[0][2] = scale.x * rotationMatrix.m[0][2];
	result.m[0][3] = 0.0f;

	result.m[1][0] = scale.y * rotationMatrix.m[1][0];
	result.m[1][1] = scale.y * rotationMatrix.m[1][1];
	result.m[1][2] = scale.y * rotationMatrix.m[1][2];
	result.m[1][3] = 0.0f;

	result.m[2][0] = scale.z * rotationMatrix.m[2][0];
	result.m[2][1] = scale.z * rotationMatrix.m[2][1];
	result.m[2][2] = scale.z * rotationMatrix.m[2][2];
	result.m[2][3] = 0.0f;

	// 平行移動を適用
	result.m[3][0] = translate.x;
	result.m[3][1] = translate.y;
	result.m[3][2] = translate.z;
	result.m[3][3] = 1.0f;

	return result;
}

//正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result{};

	result.m[0][0] = 2 / (right - left);
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 2 / (top - bottom);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = 1 / (farClip - nearClip);
	result.m[2][3] = 0;

	result.m[3][0] = (left + right) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = nearClip / (nearClip - farClip);
	result.m[3][3] = 1;





	return result;
}
//透視射影行列
Matrix4x4 MakePerspectiveFovMatrix(float forY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result{};

	result.m[0][0] = (1 / aspectRatio) * 1 / std::tan(forY / 2);
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = 1 / std::tan(forY / 2);
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = farClip / (farClip - nearClip);
	result.m[2][3] = 1;

	result.m[3][0] = 0;
	result.m[3][1] = 0;
	result.m[3][2] = (-nearClip * farClip) / (farClip - nearClip);
	result.m[3][3] = 0;

	return result;
}
//ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float leht, float top, float width, float height, float minDepth, float maxDepth) {
	Matrix4x4 result{};

	result.m[0][0] = width / 2;
	result.m[0][1] = 0;
	result.m[0][2] = 0;
	result.m[0][3] = 0;

	result.m[1][0] = 0;
	result.m[1][1] = (-height) / 2;
	result.m[1][2] = 0;
	result.m[1][3] = 0;

	result.m[2][0] = 0;
	result.m[2][1] = 0;
	result.m[2][2] = maxDepth - minDepth;
	result.m[2][3] = 0;

	result.m[3][0] = leht + (width / 2);
	result.m[3][1] = top + (height / 2);
	result.m[3][2] = minDepth;
	result.m[3][3] = 1;

	return result;
};
//座標変換
Vector3 Transforms(const Vector3& vector, const Matrix4x4& matrix) {
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

Vector4 Transforms(const Vector4& vec, const Matrix4x4& mat)
{
	return Vector4{
		vec.x * mat.m[0][0] + vec.y * mat.m[1][0] + vec.z * mat.m[2][0] + vec.w * mat.m[3][0],
		vec.x * mat.m[0][1] + vec.y * mat.m[1][1] + vec.z * mat.m[2][1] + vec.w * mat.m[3][1],
		vec.x * mat.m[0][2] + vec.y * mat.m[1][2] + vec.z * mat.m[2][2] + vec.w * mat.m[3][2],
		vec.x * mat.m[0][3] + vec.y * mat.m[1][3] + vec.z * mat.m[2][3] + vec.w * mat.m[3][3]
	};
}

Vector3 TransformNormal(const Vector3& v, const Matrix4x4& m) {
	Vector3 result{

		v.x * m.m[0][0] + v.y * m.m[1][0] + v.z * m.m[2][0],
		v.x * m.m[0][1] + v.y * m.m[1][1] + v.z * m.m[2][1],
		v.x * m.m[0][2] + v.y * m.m[1][2] + v.z * m.m[2][2],
	};

	return result;
}


Vector3 ClosestPoint(const Vector3& point, const Segment& segment)
{
	Vector3 result{};
	Vector3 d = segment.diff();
	Vector3 v = {
		point.x - segment.origin.x,
		point.y - segment.origin.y,
		point.z - segment.origin.z 
	};


	float dot = Dot(d, d);
	if (dot == 0.0f) {
		return segment.origin;
	}

	float t = Dot(v, d) / dot;

	t = Clamp(t);

	result.x = segment.origin.x + d.x * t;
	result.y = segment.origin.y + d.y * t;
	result.z = segment.origin.z + d.z * t;

	return result;
}
//
Vector3 ClosestPointAABBSphere(const Sphere& sphere, const AABB& aabb)
{
	Vector3 closestPoint;
	closestPoint.x = Clamp(sphere.center.x, aabb.min_.x, aabb.max_.x);
	closestPoint.y = Clamp(sphere.center.y, aabb.min_.y, aabb.max_.y);
	closestPoint.z = Clamp(sphere.center.z, aabb.min_.z, aabb.max_.z);

	return closestPoint;
}



// 点と線分間の距離の二乗を求める関数
float PointLineDistanceSquared(const Vector3& point, const Vector3& a, const Vector3& b)
{
	Vector3 ab = b - a;
	Vector3 ap = point - a;
	float abLengthSquared = ab.LengthSq();

	if (abLengthSquared == 0.0f) {
		return ap.LengthSq(); // 線分が点の場合
	}

	float t = ap.Dot(ab) / abLengthSquared;
	t = std::max(0.0f, std::min(1.0f, t));

	Vector3 closestPoint = a + ab * t;
	return (point - closestPoint).LengthSq();
}

// 線分と線分間の距離の二乗を求める関数
float SegmentSegmentDistanceSquared(const Segment& seg1, const Segment& seg2)
{
	Vector3 u = seg1.diff();
	Vector3 v = seg2.diff();
	Vector3 w = seg1.origin - seg2.origin;

	float a = u.Dot(u);  // |u|^2
	float b = u.Dot(v);
	float c = v.Dot(v);  // |v|^2
	float d = u.Dot(w);
	float e = v.Dot(w);

	float denom = a * c - b * b;
	float s, t;

	if (denom != 0.0f) {
		s = (b * e - c * d) / denom;
		t = (a * e - b * d) / denom;
		s = std::max(0.0f, std::min(1.0f, s));
		t = std::max(0.0f, std::min(1.0f, t));
	}
	else {
		s = 0.0f;
		t = d / b;
		t = std::max(0.0f, std::min(1.0f, t));
	}

	Vector3 closestPoint1 = seg1.origin + u * s;
	Vector3 closestPoint2 = seg2.origin + v * t;
	return (closestPoint1 - closestPoint2).LengthSq();
}


//
Plane PlaneFromPoints(const Vector3& p1, const Vector3& p2, const Vector3& p3) {
	Plane result{};

	// 2つのベクトルを求める
	Vector3 v1 = Subtract(p2, p1);
	Vector3 v2 = Subtract(p3, p1);

	// 法線を計算 (外積)
	result.normal = Cross(v1, v2);

	// ゼロベクトルチェック (3点が同一直線上の場合)
	if (Length(result.normal) == 0.0f) {
		// 法線が求まらない場合のエラーハンドリング
		result.normal = { 0.0f, 0.0f, 0.0f };
		result.distance = 0.0f;
		return result;
	}

	// 正規化
	result.normal = Normalize(result.normal);

	// 平面の距離 D の計算 (符号の修正)
	result.distance = -Dot(result.normal, p1);

	return result;
}

Vector3 Reflect(const Vector3& input, const Vector3& normal)
{
	Vector3 result;
	float dotProduct = Dot(input, normal);
	result.x = input.x - normal.x * (2 * dotProduct);
	result.y = input.y - normal.y * (2 * dotProduct);
	result.z = input.z - normal.z * (2 * dotProduct);
	return result;
}

Vector3 Reflect(const Vector3& input, const Vector3& normal, float restitution)
{
	// まず normal が正規化されている前提だが、明示的に normalize する場合:
	Vector3 norm = Normalize(normal);

	// 計算用変数
	float scale = 2.0f * Dot(input, norm) * restitution;

	// 反射ベクトルの計算
	return input - norm * scale;
}

Vector3 ClosestPointSegmentTriangle(const Segment& segment, const Triangle& triangle) {
	// 三角形の3辺を定義
	Segment edge1 = { triangle.vertices[0], triangle.vertices[1] };
	Segment edge2 = { triangle.vertices[1], triangle.vertices[2] };
	Segment edge3 = { triangle.vertices[2], triangle.vertices[0] };

	// 最初に線分の始点を初期最近接点とする
	Vector3 closest = ClosestPointSegment(segment, triangle.vertices[0]);

	// 三角形のエッジに対して最近接点を更新
	closest = ClosestPointSegmentSegment(segment, edge1, closest);
	closest = ClosestPointSegmentSegment(segment, edge2, closest);
	closest = ClosestPointSegmentSegment(segment, edge3, closest);

	// 三角形の面上の最近接点も考慮（重要）
	Plane plane = PlaneFromPoints(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]);
	Vector3 pointOnPlane = ClosestPointOnPlane(plane, segment.origin);
	closest = LengthSquared(Subtract(closest, segment.origin)) < LengthSquared(Subtract(pointOnPlane, segment.origin))
		? closest
		: pointOnPlane;

	return closest;
}

Vector3 ClosestPointSegmentSegment(const Segment& seg1, const Segment& seg2, Vector3 currentClosest) {
	Vector3 u = seg1.diff();
	Vector3 v = seg2.diff();
	Vector3 w = Subtract(seg1.origin, seg2.origin);

	float a = Dot(u, u);
	float b = Dot(u, v);
	float c = Dot(v, v);
	float d = Dot(u, w);
	float e = Dot(v, w);

	float denom = a * c - b * b;

	// ゼロ除算の回避
	float s = 0.0f, t = 0.0f;
	if (denom != 0.0f) {
		s = Clamp3((b * e - c * d) / denom, 0.0f, 1.0f);
		t = Clamp3((a * e - b * d) / denom, 0.0f, 1.0f);
	}

	Vector3 closestOnSeg1 = Add(seg1.origin, Multiply(u, s));
	Vector3 closestOnSeg2 = Add(seg2.origin, Multiply(v, t));

	return LengthSquared(Subtract(closestOnSeg1, closestOnSeg2)) < LengthSquared(Subtract(currentClosest, seg1.origin))
		? closestOnSeg1
		: currentClosest;
}

Vector3 ClosestPointSegment(const Segment& segment, const Vector3& point) {
	Vector3 diff = segment.diff();
	float lenSq = Dot(diff, diff);

	if (lenSq == 0.0f) {
		return segment.origin;  // 線分が点の場合は始点を返す
	}

	float t = Dot(Subtract(point, segment.origin), diff) / lenSq;
	t = Clamp3(t, 0.0f, 1.0f);

	return Add(segment.origin, Multiply(diff, t));
}

Vector3 ClosestPointOnPlane(const Plane& plane, const Vector3& point) {
	// 平面の法線ベクトル
	Vector3 normal = plane.normal.Normalize();

	// 点から平面までの距離 d = dot(N, P) - D
	float distance = Dot(normal, point) - plane.distance;

	// 最近接点を計算: P - d * N
	Vector3 closestPoint = Subtract(point, Multiply(normal, distance));

	return closestPoint;
}



float SegmentClosestDistanceSq(const Segment& seg0, const Segment& seg1) {
	Vector3 u = seg0.diff();
	Vector3 v = seg1.diff();
	Vector3 w = seg0.origin - seg1.origin;

	float a = u.Dot(u); // seg0の長さの2乗
	float b = u.Dot(v);
	float c = v.Dot(v); // seg1の長さの2乗
	float d = u.Dot(w);
	float e = v.Dot(w);

	float denom = a * c - b * b;
	float sN, sD = denom;
	float tN, tD = denom;

	// デノミネーターがゼロの場合（平行な場合）、近似値をとる
	if (denom < 1e-6f) {
		sN = 0.0f;
		sD = 1.0f;
		tN = e;
		tD = c;
	}
	else {
		sN = (b * e - c * d);
		tN = (a * e - b * d);

		if (sN < 0.0f) {
			sN = 0.0f;
			tN = e;
			tD = c;
		}
		else if (sN > sD) {
			sN = sD;
			tN = e + b;
			tD = c;
		}
	}

	if (tN < 0.0f) {
		tN = 0.0f;
		if (-d < 0.0f) {
			sN = 0.0f;
		}
		else if (-d > a) {
			sN = sD;
		}
		else {
			sN = -d;
			sD = a;
		}
	}
	else if (tN > tD) {
		tN = tD;
		if ((-d + b) < 0.0f) {
			sN = 0.0f;
		}
		else if ((-d + b) > a) {
			sN = sD;
		}
		else {
			sN = (-d + b);
			sD = a;
		}
	}

	float sc = (std::abs(sN) < 1e-6f ? 0.0f : sN / sD);
	float tc = (std::abs(tN) < 1e-6f ? 0.0f : tN / tD);

	Vector3 dP = w + (u * sc) - (v * tc);
	return dP.LengthSq();
}





Vector3 CatmullRom(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t) {
	const float s = 0.5f; // Catmull-Rom スプラインのスケール

	float t2 = t * t;  // tの2乗
	float t3 = t2 * t; // tの3乗

	// 係数の計算
	Vector3 e3 = {
		(-p0.x + 3 * p1.x - 3 * p2.x + p3.x),
		(-p0.y + 3 * p1.y - 3 * p2.y + p3.y),
		(-p0.z + 3 * p1.z - 3 * p2.z + p3.z)
	};

	Vector3 e2 = {
		(2 * p0.x - 5 * p1.x + 4 * p2.x - p3.x),
		(2 * p0.y - 5 * p1.y + 4 * p2.y - p3.y),
		(2 * p0.z - 5 * p1.z + 4 * p2.z - p3.z)
	};

	Vector3 e1 = {
		(-p0.x + p2.x),
		(-p0.y + p2.y),
		(-p0.z + p2.z)
	};

	Vector3 e0 = {
		(2 * p1.x),
		(2 * p1.y),
		(2 * p1.z)
	};

	// Catmull-Rom 補間計算
	Vector3 pt = {
		(e3.x * t3 + e2.x * t2 + e1.x * t + e0.x) * s,
		(e3.y * t3 + e2.y * t2 + e1.y * t + e0.y) * s,
		(e3.z * t3 + e2.z * t2 + e1.z * t + e0.z) * s
	};

	return pt;
}


Vector3 CatmullRom(const std::vector<Vector3>& points, float t) {
	assert(points.size() >= 4 && "制御点は4点以上必要です");

	// 区間数（`N-1` の計算）
	size_t division = points.size() - 1;
	if (division == 0) return points[0]; // 例外ケース処理

	// 1区間の長さ（全体を1.0とした場合）
	float areaWidth = 1.0f / division;

	// `t` を `areaWidth` で割り、区間番号を取得
	size_t index = static_cast<size_t>(t / areaWidth);
	index = Clamp(index, static_cast<size_t>(0), division - 1);

	// 各点のインデックス
	size_t index0 = (index == 0) ? 0 : index - 1;
	size_t index1 = index;
	size_t index2 = index + 1;
	size_t index3 = index + 2;

	// `index3` の制限 (最後の区間では p3 = p2)
	if (index3 >= points.size()) index3 = points.size() - 1;

	// 4点の座標
	const Vector3& p0 = points[index0];
	const Vector3& p1 = points[index1];
	const Vector3& p2 = points[index2];
	const Vector3& p3 = points[index3];

	// 区間内の `t` を正規化
	float t_2 = static_cast<float>(t - index * areaWidth) / areaWidth;
	t_2 = Clamp(t_2, 0.0f, 1.0f);

	// 4点を指定して Catmull-Rom 補間
	return CatmullRom(p0, p1, p2, p3, t_2);
}



// カーブ上の点を取得 (Catmull-Rom)
Vector3 CatmullRom2(const std::vector<Vector3>& controlPoints, float t) {
	int p0, p1, p2, p3;
	p1 = static_cast<int>(t);
	p2 = (p1 + 1) % (int)controlPoints.size();
	p3 = (p2 + 1) % (int)controlPoints.size();
	p0 = (p1 - 1 + (int)controlPoints.size()) % (int)controlPoints.size();

	t = t - static_cast<int>(t); // 小数部分のみを取り出す

	float t2 = t * t;
	float t3 = t2 * t;

	Vector3 result = Add(
		Add(
			Multiply(controlPoints[p0], -0.5f * t3 + t2 - 0.5f * t),
			Multiply(controlPoints[p1], 1.5f * t3 - 2.5f * t2 + 1.0f)),
		Add(
			Multiply(controlPoints[p2], -1.5f * t3 + 2.0f * t2 + 0.5f * t),
			Multiply(controlPoints[p3], 0.5f * t3 - 0.5f * t2)
		)
	);
	return result;
}

// アーク長を計算する関数
float CalculateArcLength(const std::vector<Vector3>& controlPoints, int numSamples) {
	float totalLength = 0.0f;
	Vector3 prevPos = CatmullRom(controlPoints, 0.0f);

	for (int i = 1; i <= numSamples; ++i) {
		float t = static_cast<float>(i) / numSamples;
		Vector3 currPos = CatmullRom(controlPoints, t);
		totalLength += Distance(prevPos, currPos);
		prevPos = currPos;
	}

	return totalLength;
}

// アーク長に基づく位置を取得する関数
float FindTByArcLength(const std::vector<Vector3>& controlPoints, float targetLength, int numSamples) {
	float currentLength = 0.0f;
	Vector3 prevPos = CatmullRom(controlPoints, 0.0f);

	for (int i = 1; i <= numSamples; ++i) {
		float t = static_cast<float>(i) / numSamples;
		Vector3 currPos = CatmullRom(controlPoints, t);
		currentLength += Distance(prevPos, currPos);

		if (currentLength >= targetLength) {
			return t;
		}
		prevPos = currPos;
	}

	return 1.0f; // 最後まで到達した場合
}


// 曲線を細かくサンプリングし、累積アーク長を計算
std::vector<std::pair<float, float>> CalculateArcLengths(const std::vector<Vector3>& controlPoints, int numSamples) {
	std::vector<std::pair<float, float>> arcLengths;
	arcLengths.push_back({ 0.0f, 0.0f });

	float totalLength = 0.0f;
	Vector3 prevPos = CatmullRom(controlPoints, 0.0f);

	for (int i = 1; i <= numSamples; ++i) {
		float t = static_cast<float>(i) / numSamples;
		Vector3 currPos = CatmullRom(controlPoints, t);
		totalLength += Distance(prevPos, currPos);
		arcLengths.push_back({ t, totalLength });
		prevPos = currPos;
	}

	return arcLengths;
}

// アーク長からtを逆算する関数
float GetTFromArcLength(const std::vector<std::pair<float, float>>& arcLengths, float targetLength) {
	for (size_t i = 1; i < arcLengths.size(); ++i) {
		if (arcLengths[i].second >= targetLength) {
			float t1 = arcLengths[i - 1].first;
			float t2 = arcLengths[i].first;
			float l1 = arcLengths[i - 1].second;
			float l2 = arcLengths[i].second;

			// 線形補間でtを求める
			return t1 + (targetLength - l1) / (l2 - l1) * (t2 - t1);
		}
	}
	return 1.0f;
}

// 曲率を計算する関数
float Curvature(const Vector3& p0, const Vector3& p1, const Vector3& p2) {
	Vector3 v1 = Subtract(p1, p0);
	Vector3 v2 = Subtract(p2, p1);
	float angle = std::acos(Dot(Normalize(v1), Normalize(v2)));
	return angle / Distance(p0, p1);
}

// 曲率に基づくアダプティブサンプリング
std::vector<float> AdaptiveSampling(const std::vector<Vector3>& controlPoints, int baseSamples) {
	std::vector<float> samplePoints;
	samplePoints.push_back(0.0f);

	for (int i = 1; i < baseSamples; ++i) {
		float t = static_cast<float>(i) / baseSamples;
		Vector3 p0 = CatmullRom2(controlPoints, std::max(t - 0.01f, 0.0f));
		Vector3 p1 = CatmullRom2(controlPoints, t);
		Vector3 p2 = CatmullRom2(controlPoints, std::min(t + 0.01f, 1.0f));

		float curvature = Curvature(p0, p1, p2);
		float sampleRate = std::clamp(1.0f / (curvature + 0.1f), 0.01f, 0.2f);
		samplePoints.push_back(samplePoints.back() + sampleRate);
	}

	return samplePoints;
}


float DegreesToRadians(float degrees) { return float(degrees * ((float)M_PI / 180.0)); }

float RadiansToDegrees(float radians) { return float(radians * (180.0 / (float)M_PI)); }


#pragma endregion //数学関数