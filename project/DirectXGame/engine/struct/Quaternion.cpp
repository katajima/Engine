#include"Quaternion.h"

// + 演算子のオーバーロード
Quaternion Quaternion::operator+(const Quaternion& other) const {

    return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
}

// - 演算子のオーバーロード
Quaternion Quaternion::operator-(const Quaternion& other) const {
    return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
}

// * 演算子のオーバーロード（クォータニオンの乗算）
Quaternion Quaternion::operator*(const Quaternion& other) const {
    return Quaternion(
        w * other.x + x * other.w + y * other.z - z * other.y,
        w * other.y - x * other.z + y * other.w + z * other.x,
        w * other.z + x * other.y - y * other.x + z * other.w,
        w * other.w - x * other.x - y * other.y - z * other.z
    );
}
Quaternion Quaternion::operator*(const float& other) const { 
    return Quaternion(w * other, x * other, y * other, z * other); 
}

Quaternion Quaternion::operator-() const
{
    return Quaternion(-x, -y, -z, -w);
}

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
