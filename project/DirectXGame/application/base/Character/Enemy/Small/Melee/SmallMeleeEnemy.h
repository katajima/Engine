#pragma once
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include"SmallMeleeEnemyState.h"
#include "DirectXGame/application/base/Weapon/Enemy/SmallMeleeWeapon.h"

namespace Character {
/// <summary>
/// SmallMeleeEnemyを管理・実装するクラス。
/// </summary>
	class SmallMeleeEnemy : public BaseEnemy {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		SmallMeleeEnemy() {}

		// 初期化
		void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;


		void Reload() override { ReloadComboData(); };

	private:
		/// <summary>
		/// ステートマシーン初期化
		/// </summary>
		void InitStateMachine() override;

		/// <summary>
		/// リロード
		/// </summary>
		void ReloadComboData();

	};
}
