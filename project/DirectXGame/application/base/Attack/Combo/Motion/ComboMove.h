#pragma once
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
#include <DirectXGame/engine/struct/Vector3.h>


// 前方宣言
namespace Engine {
	class AnimationComponent;	// アニメーション
	class RigidBodyComponent;	// リジットボディー
	class Camera;
	class WorldTransform;
}

class MovementComponent;	// 移動関係
namespace Character {
	class BaseCharacter;		// キャラクター
}

class BaseWeapon;			// 武器
class JumpSystem;			// ジャンプシステム
class MoveRequestSystem;	// 移動リクエストシステム
class LockOnSystem;			// ロックオンシステム

namespace Combo {

	class ComboMove {
	public:
		// 開始
		void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx);

		// 更新
		void Update(const Character::CharacterContext& ctx, float timer);

		// 終了
		void Exit(Character::BaseCharacter* owner);

	public:
		// 移動できるか
		bool IsMove() const { return isMove_; }
		// データ構造体取得
		GlobalMove& GetData() { return data_; }
		// 方向取得
		Vector3 GetDirection() const { return direction_; }
		// 方向指定
		void SetDirection(const Vector3& dire) { direction_ = dire; }
		// ターゲット位置取得
		Vector3 GetTargetPosition() const { return targetPos_; }
		// ターゲット
		const Engine::WorldTransform* GetTarget();
	private:
		// 移動タイプによる処理
		void MoveTypeProcess(float timer, float dt);
		// 移動タイプによる方向指定処理
		void MoveTypeDirectionProcess();
		// ローカル移動ベクトルを基準方向へ変換した最終方向を作る
		Vector3 BuildMoveDirection() const;
	private:
		// 移動
		MovementComponent* moveComponent = nullptr;
		// ワールドトランスフォーム
		Engine::WorldTransform* worldTransform = nullptr;
		// ワールドトランスフォーム（ターゲット）
		const Engine::WorldTransform* targetWorldTransform = nullptr;


		// ロックオンシステム
		LockOnSystem* lockOnSystem = nullptr;
		// ターゲット
		const Character::BaseCharacter* traget = nullptr;
		// 攻撃移動システム
		MoveRequestSystem* moveRequestSystem = nullptr;
		//
		const Engine::Camera* camera = nullptr;
	private:
		// データ
		GlobalMove data_;
		// 方向
		Vector3 direction_ = {};
		// 最終移動方向
		Vector3 moveDirection_ = {};
		// 移動出来るか
		bool isMove_ = true;
	private:
		// ターゲット位置
		Vector3 targetPos_ = {};
		// スティック方向
		Vector2 stickDirection_ = {};
	};
}
