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
		// コンストラクタ
		SmallRangeEnemy() {}

		// 初期化
		void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager, 
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

		// 毎フレーム更新
		void Update() override;

	private:
		// ステートマシーン初期化
		void InitStateMachine() override;

	private:
		std::unique_ptr<ObjectComponent> objectComponentPropeller_ = nullptr;
	};
}