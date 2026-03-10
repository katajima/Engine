#pragma once
#include "DirectXGame/application/base/Input/InputData.h"	// 入力データ
#include "CharacterData.h"									// キャラクターのパラメータ
#include "DirectXGame/application/base/Object/ObjectComponent.h"	// オブジェクト
#include "DirectXGame/application/base/State/BaseMainState.h"

// 前方宣言
class MovementComponent;
class JumpSystem;
class LockOnSystem;

namespace Combo {
	class StateMachine;
}


namespace Character {

	// 前方宣言
	class BaseCharacter;
	
	/// <summary>
	/// キャラクターコンテキスト
	/// </summary>
	struct CharacterContext {
		// 入力データ
		PlayerInputData inputData;
		// スティック方向
		Vector2 worldStickDirection{};
		// 基礎パラメータ
		const BasicParameters* parameters = nullptr;
		// 時間取得
		float dt = 0.0f;
		// カメラ
		const Engine::Camera* camera = nullptr;
		Vector3 cameraDirection{};
		// 位置
		Vector3 position{};
		// 方向
		Vector3 direction{};
		// 速度
		Vector3 velocity{};
		// ステート 
		Character::CharacterMainState state = Character::CharacterMainState::Idle;

		// 着地状態か
		bool onGround = false;
		// 攻撃中
		bool isAttacking = false;
		// スタン状態か
		bool isHitStun = false;
		// 重力有効か
		bool isGravity = true;

		// 
		float skyHeight = 0.0f;

		// 上昇時の重力係数
		float upGravity = 15.0f;
		// 落下時の重力係数
		float fallGravity = 30.0f;
		// 攻撃中の重力係数
		float attackingGravity = 20.0f;

		// 移動スピード
		float moveSpeed = 1.0f;
		// ターゲット指定
		const BaseCharacter* target = nullptr;
	};

	/// <summary>
	/// キャラクターのコンテキストを管理するクラス
	/// </summary>
	class CharacterContextSystem {
	public:

		// 初期化
		void Initialize(BaseCharacter* owner,const InputSystem* input);

		CharacterContext CreateContext(float dt);

	private:
		// キャラクター（所有者）
		const BaseCharacter* owner = nullptr;
		// 入力データ
		const InputSystem* input = nullptr;
		// ワールドトランスフォーム
		const Engine::WorldTransform* worldTransform = nullptr;
		// リジットボディー
		const Engine::RigidBodyComponent* rigidBody = nullptr;
		// 移動
		const MovementComponent* movementComponent = nullptr;
		// ジャンプシステム
		const JumpSystem* jumpSystem = nullptr;
		// ロックオンシステム
	 	const LockOnSystem* lockOnSystem = nullptr;
		// コンボステートマシン
		const Combo::StateMachine* comboStateMachine = nullptr;
	};


}