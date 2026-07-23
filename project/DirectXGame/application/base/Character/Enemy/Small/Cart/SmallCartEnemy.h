#pragma once
#include"DirectXGame/application/base/Character/Enemy/Base/BaseEnemy.h"
#include"SmallCartEnemyState.h"

namespace Character {

/// <summary>
/// SmallCartEnemyを管理・実装するクラス。
/// </summary>
	class SmallCartEnemy : public BaseEnemy {
	public:
		/// <summary>
		/// コンストラクタ
		/// </summary>
		SmallCartEnemy() {

		}

		// 初期化
		void Initialize(InputSystem* inputSystem, Engine::EntityManager* entityManager,
			Engine::GlobalVariables* globalVariables, Vector3 position, Engine::Camera* camera) override;

		/// <summary>
		/// 毎フレーム更新
		/// </summary>
		void Update() override;

		/// <summary>
		/// エフェクト描画
		/// </summary>
		void DrawEffect() override {};
		/// <summary>
		/// 描画2d
		/// </summary>
		void Draw2D() override;
		/// <summary>
		/// 移動
		/// </summary>
		void Move() override;
		/// <summary>
		/// ジャンプ
		/// </summary>
		void Jump() override {};
	private:
		/// <summary>
		/// ステートマシーン初期化
		/// </summary>
		void InitStateMachine() override;

	private:
		//
		float attackTimer_ = 0.0f;
		//
		float moveSpeed_ = 1.0f;
	};
}
