#pragma once
#include"DirectXGame/application/base/Character/Base/Enemy/BaseEnemy.h"
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
		void Initialize(Engine::Input* input, Engine::Entity3DManager* entity3DManager, Engine::Entity2DManager* entity2DManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

		// 毎フレーム更新
		void Update() override;

		// エフェクト描画
		void DrawEffect() override {};
		// 描画2d
		void Draw2D() override;
		// エミッター出現
		void Emit() override;
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
		float cooldownTime_ = 5.0f;
		//
		float attackTimer_ = 0.0f;
	};
}