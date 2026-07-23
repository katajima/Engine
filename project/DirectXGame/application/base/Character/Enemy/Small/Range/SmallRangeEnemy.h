#pragma once
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include "SmallRangeEnemyState.h"
#include <DirectXGame/application/base/Weapon/Enemy/SmallRangeWeapon.h>

namespace Character {
/// <summary>
/// SmallRangeEnemyを管理・実装するクラス。
/// </summary>
	class SmallRangeEnemy : public BaseEnemy {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		SmallRangeEnemy() {}

		// 初期化
		void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

		/// <summary>
		/// 毎フレーム更新
		/// </summary>
		void Update() override;

	private:
		/// <summary>
		/// ステートマシーン初期化
		/// </summary>
		void InitStateMachine() override;

	private:
		std::unique_ptr<ObjectComponent> objectComponentPropeller_ = nullptr;
	};
}