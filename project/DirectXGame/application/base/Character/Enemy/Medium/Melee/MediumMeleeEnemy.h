#pragma once
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "MediumMeleeEnemyState.h"
#include "DirectXGame/application/base/Weapon/Enemy/SmallMeleeWeapon.h"

namespace Character {
	/// <summary>
	/// 通常敵クラス
	/// </summary>
	class MediumMeleeEnemy : public BaseEnemy {
	public:
		// コンストラクタ
		MediumMeleeEnemy() {}

		// 初期化
		void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

	private:
		// ステートマシーン初期化
		void InitStateMachine() override;
	};
}