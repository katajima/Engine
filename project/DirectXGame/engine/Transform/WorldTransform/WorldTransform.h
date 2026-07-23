#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"


namespace Engine {
	/// <summary>
	/// ワールドトランスフォームクラス
	/// </summary>
	class WorldTransform
	{
	public:
		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize();
		/// <summary>
		/// 更新
		/// </summary>
		void Update();
		/// <summary>
		/// 親子関係
		/// </summary>
		void SetParent(const Matrix4x4& parent) {
			isParent = true;
			parentMatrix_ = parent;
		};

		/// <summary>
		/// ワールド座標取得
		/// </summary>
		Vector3 GetWorldPosition() const {
			return worldMat_.GetWorldPosition();
		}
		/// <summary>
		/// 1フレーム前ワールド座標取得
		/// </summary>
		Vector3 GetPreWorldPosition() const {
			return worldPreMat_.GetWorldPosition();
		}

		/// <summary>
		/// 向いている方向取得
		/// </summary>
		Vector3 GetForward() const {
			Matrix4x4 rotMat = MakeRotateXYZ(rotate_);

			// Z軸がForward
			return Vector3{
				rotMat.m[2][0],
				rotMat.m[2][1],
				rotMat.m[2][2]
			};
		}

	public:
		Vector3 scale_ = { 1,1,1 };		// 拡縮
		Vector3 rotate_ = { 0,0,0 };		// 回転
		Vector3 translate_ = { 0,0,0 }; // 位置

		// マトリックス(現在)
		Matrix4x4 worldMat_{};
		// マトリックス(過去)
		Matrix4x4 worldPreMat_{};

		// 親となるワールド変換へのポインタ
		WorldTransform* parent_ = nullptr;

	private:
		bool isParent = false;
		Matrix4x4 parentMatrix_{};
	};
}
