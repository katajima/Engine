#pragma once
#include <vector>
#include <DirectXGame/application/base/Character/Enemy/Base/EnemyData.h>
#include <DirectXGame/engine/struct/Vector3.h>
#include "EnemyAttackRequest.h"
#include <memory>

namespace Character {

	class CharacterStateMachine;

	// 敵の攻撃システム(攻撃順番など)
/// <summary>
/// EnemyAttackSystemを管理・実装するクラス。
/// </summary>
	class EnemyAttackSystem {
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(CharacterStateMachine* characterStateMachine);
		// 更新
		void Update(
			float dt,
			const Vector3& ownerPos,
			const Vector3& targetPos,
			const Vector3& slotPos,
			float& speed,
			float& rotateY,
			const EnemyGlobalData& data,
			float moveSpeed
		);
		
		/// <summary>
		/// 攻撃要求の取得
		/// </summary>
		EnemyAttackRequest* GetAttackRequest() const;

	private:
		CharacterStateMachine* characterStateMachine = nullptr;
		float timer_ = 0.0f;
		// 攻撃要求
		std::unique_ptr<EnemyAttackRequest> attackRequest_;
	};
}