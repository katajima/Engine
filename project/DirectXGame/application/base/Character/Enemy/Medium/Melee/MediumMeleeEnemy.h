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

		// 毎フレーム更新
		void Update() override;

		// エフェクト描画
		void DrawEffect() override {};
		// 描画2d
		void Draw2D() override;
		// 移動
		void Move() override;
		// ジャンプ
		void Jump() override {};
		// 攻撃
		void Attack() override {};

	private:
		// ステートマシーン初期化
		void InitStateMachine() override;
		// パーティクル初期化
		void InitParticle();

	private:
		//
		float moveSpeed_ = 1.0f;
	};
}