#pragma once
#include"DirectXGame/engine/math/MathFunctions.h"
#include "DirectXGame/engine/Transform/WorldTransform/WorldTransform.h"
#include "DirectXGame/engine/Transform/Transformation/Transformation.h"


namespace Engine {
	/// <summary>
	/// トランスフォームコンポーネントクラス
	/// </summary>
	class TransformComponent
	{
	public:

		// 初期化
		void Init() {
			worldTransform_.Initialize();
			worldTransform_.translate_.x = { 0.00000001f };
		}
		// 更新
		void Update() {
			worldTransform_.Update();
		}
		// ワールドトランスフォーム取得
		WorldTransform& GetWorldTransform() { return worldTransform_; }

		// ワールド座標
		Vector3 GetWorldPosition() const {
			// ワールド座標を入れる
			Vector3 worldPos;
			worldPos.x = worldTransform_.worldMat_.m[3][0];
			worldPos.y = worldTransform_.worldMat_.m[3][1];
			worldPos.z = worldTransform_.worldMat_.m[3][2];
			return worldPos;
		};

		// ワールド座標
		Vector3 GetPreWorldPosition() const {
			// ワールド座標を入れる
			Vector3 worldPos;
			worldPos.x = worldTransform_.worldPreMat_.m[3][0];
			worldPos.y = worldTransform_.worldPreMat_.m[3][1];
			worldPos.z = worldTransform_.worldPreMat_.m[3][2];
			return worldPos;
		};

	private:
		WorldTransform worldTransform_;
	};
}

