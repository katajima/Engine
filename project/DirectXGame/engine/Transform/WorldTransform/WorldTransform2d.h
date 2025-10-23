#pragma once
#include"DirectXGame/engine/math/MathFanctions.h"

/// <summary>
/// ワールドトランスフォーム2dクラス
/// </summary>
class WorldTransform2d
{
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 親子関係
	void SetParent(const Matrix3x3& parent) {
		isPearent = true;
		parentMatrix_ = parent;
	};
	// 子設定
	void SetChild(WorldTransform2d* child)
	{
		child->parent_ = this;
	}
	// ワールド位置取得
	Vector2 GetWorldPosition() const { return worldMat_.GetWorldPosition(); }
	// マトリックス4x4に変換
	Matrix4x4 GetConvert2DMatrixTo4x4() const { return Convert2DMatrixTo4x4(worldMat_); }
	// 回転取得
	float GetWorldRotation() const {
		if (parent_) {
			return parent_->GetWorldRotation() + rotate_;
		}
		return rotate_;
	}

public:
	Vector2 scale_ = { 1,1};		// 拡縮
	float rotate_ = 0;				// 回転
	Vector2 translate_ = { 0,0};	// 位置

	// マトリックス(現在)
	Matrix3x3 worldMat_;
	// マトリックス(過去)
	Matrix3x3 worldPreMat_;

	// 親となるワールド変換へのポインタ
	WorldTransform2d* parent_ = nullptr;
private:
	bool isPearent = false;
	Matrix3x3 parentMatrix_;
};