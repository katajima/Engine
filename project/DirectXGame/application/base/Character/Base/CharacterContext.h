#pragma once
#include "DirectXGame/application/base/Input/InputData.h"	// 入力データ
#include "CharacterData.h"									// キャラクターのパラメータ
#include "DirectXGame/application/base/Character/State/MainState.h"
#include "DirectXGame/engine/struct/Vector3.h"				// Vector3

// 前方宣言
class MovementComponent;
class JumpSystem;
class MoveSystem;
class DeathSystem;
class LockOnSystem;
class HitMotionSystem;
class ObjectComponent;
class InputSystem;
namespace Combo {
	class StateMachine;
}
namespace Engine {
	class Camera;
	class RigidBodyComponent;
	class WorldTransform;
	class AnimationComponent;
}

namespace Character {

	// 前方宣言
	class BaseCharacter;

	/// <summary>
	/// キャラクターコンテキスト
	/// </summary>
	struct CharacterContext {
		const InputSystem* input = nullptr;
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
		// ヒットストップ状態か(相手が攻撃を当てたとき)
		bool isHitStop = false;
		// ヒットストップ状態か(自分が攻撃を当てたとき)
		bool isSelfHitStop = false;
		// 重力有効か
		bool isGravity = true;
		// 停止するか
		bool isStop = false;
		// ジャンプ中か
		bool isJumping = false;
		// ジャンプ攻撃中か
		bool isJumpAttacking = false;
		// 必殺技中か
		bool isSpecialAttacking = false;
		// 移動可能か
		bool isCanMove = true;
		// ジャンプ可能か
		bool isCanJump = false;
		// 被弾中か
		bool isDamage = false;
		// ダッシュ中か
		bool isDashing = false;

		// 高さ
		float skyHeight = 1.5f;

		// 上昇時の重力係数
		float upGravity = 1.0f;
		// 落下時の重力係数
		float fallGravity = 2.0f;
		// 攻撃中の重力係数
		float attackingGravity = 20.0f;
		// 攻撃中の最大落下速度。0以下なら制限なし。
		float attackingMaxFallSpeed = 0.0f;
		// 被弾中の重力係数
		float damageGravity = 1.0f;
		// 死亡中の重力係数
		float dieGravity = 1.0f;

		// 移動スピード
		float moveSpeed = 1.0f;
		// ターゲット指定
		const BaseCharacter* target = nullptr;
		// AIなどが通常ターゲット以外へ移動させたい時の目標座標
		Vector3 moveTarget{};
		// moveTargetを使うか
		bool hasMoveTarget = false;

		// デバッグ用
		bool isDebug = false;
	};

	/// <summary>
	/// キャラクターのコンテキストを管理するクラス
	/// </summary>
	class CharacterContextSystem {
	public:

		/// <summary>
		/// 初期化
		/// </summary>
		void Initialize(BaseCharacter* owner, const InputSystem* input);

		/// <summary>
		/// 入力システム設定
		/// </summary>
		void SetInputSystem(const InputSystem* input) { this->input = input; }

		/// <summary>
		/// コンテキスト生成
		/// </summary>
		CharacterContext CreateContext(BaseCharacter* owner, float dt);
	private:

		/// <summary>
		/// キャラクターの状態コンテキストを作成
		/// </summary>
		void CreateContextState(BaseCharacter* owner, CharacterContext& ctx);

		/// <summary>
		/// キャラクターの重力コンテキストを作成
		/// </summary>
		void CreateContextGravity(BaseCharacter* owner, CharacterContext& ctx);

		/// <summary>
		/// キャラクターの移動コンテキストを作成
		/// </summary>
		void CreateContextMovement(BaseCharacter* owner, CharacterContext& ctx);

		/// <summary>
		/// キャラクターの入力コンテキストを作成
		/// </summary>
		void CreateContextInput(BaseCharacter* owner, CharacterContext& ctx);

	private:
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
		// 移動システム
		const MoveSystem* moveSystem = nullptr;
		// 死亡システム
		const DeathSystem* deathSystem = nullptr;
		// ロックオンシステム
		const LockOnSystem* lockOnSystem = nullptr;
		// ヒットリアクションシステム
		HitMotionSystem* hitMotionSystem = nullptr;
		// コンボステートマシン
		const Combo::StateMachine* comboStateMachine = nullptr;
		// パラメータ
		Character::ParameterComponent* parameters = nullptr;
	};


}
