#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"


namespace Engine {
	/// <summary>
	/// ワールドトランスフォームクラス
	/// </summary>
	class WorldTransform
	{
	public:
		// 初期化
		void Initialize();
		// 更新
		void Update();
		// 親子関係
		void SetParent(const Matrix4x4& parent) {
			isParent = true;
			parentMatrix_ = parent;
		};

		// ワールド座標取得
		Vector3 GetWorldPosition() const {
			return worldMat_.GetWorldPosition();
		}
		// 1フレーム前ワールド座標取得
		Vector3 GetPreWorldPosition() const {
			return worldPreMat_.GetWorldPosition();
		}

	public:
		Vector3 scale_ = { 1,1,1 };		// 拡縮
		Vector3 rotate_ = { 0,0,0 };		// 回転
		Vector3 translate_ = { 0,0,0 }; // 位置

		// マトリックス(現在)
		Matrix4x4 worldMat_;
		// マトリックス(過去)
		Matrix4x4 worldPreMat_;

		// 親となるワールド変換へのポインタ
		WorldTransform* parent_ = nullptr;

	private:
		bool isParent = false;
		Matrix4x4 parentMatrix_;
	};
}
