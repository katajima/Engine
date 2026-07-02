#pragma once
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include"SmallCartEnemyState.h"

namespace Character {

	class SmallCartEnemy : public BaseEnemy {
	public:
		// コンストラクタ
		SmallCartEnemy() {

		}

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
	private:
		// ステートマシーン初期化
		void InitStateMachine() override;

	private:
		//
		float attackTimer_ = 0.0f;
		//
		float moveSpeed_ = 1.0f;
	};
}
