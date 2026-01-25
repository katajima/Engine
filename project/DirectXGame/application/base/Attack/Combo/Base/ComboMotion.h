#pragma once
#include "DirectXGame/engine/struct/Vector3.h"
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "ComboCondition.h"

// 前方宣言
namespace Engine {
	class AnimationComponent;	// アニメーション
	class RigidBodyComponent;	// リジットボディー
}


class MovementComponent;	// 移動関係
class BaseCharacter;		// キャラクター
class BaseWeapon;			// 武器
class JumpSystem;			// ジャンプシステム

/// <summary>
/// コンボモーション
/// </summary>
class ComboMotion {
public:
	// データ構造体
	struct Data
	{
		float moveWindowStart_ = 0.1f;			// 移動受付スタート
		float moveWindowEnd_ = 0.5f;			// 移動受付エンド
		float speed_ = 0.0f;					// 移動速度

		bool isCompulsionMove_ = true;			// 強制的に移動

		bool isGravity_ = true;					// 空中でのコンボで重力はあるか？
		float gravityScale_ = 1.0f;				// 重力スケール

		// アニメーション
		std::string animationName_ = "no";		// アニメーション名前
		bool animationLoop_ = false;			// アニメーションループ
		float animationSpeed_ = 1.0f;			// アニメーションスピード
		float animationBlendTime_ = 0.1f;		// アニメーションブレンド時間
	};

	// 開始
	void Enter(BaseCharacter* owner);

	// 更新
	void Update(const Engine::Input& input, float timer, float dt);

	// 終了
	void Exit(BaseCharacter* owner);

public: // 取得 or 設定

	// 移動できるか
	bool IsMove() const { return isMove_; }

	// 移動可能時間設定
	void MoveStartEnd(float start, float end) {
		data_.moveWindowStart_ = start;
		data_.moveWindowEnd_ = end;
	};

	// アニメーション設定
	void SetAnimation(Engine::AnimationComponent* anima) { animationComponent = anima; };

	// 移動設定
	void SetMove(MovementComponent* move) { moveComponent = move; };

	// ワールドトランスフォーム設定
	void SetWorld(Engine::WorldTransform* world) { worldTransform = world; };

	// リジットボディー設定
	void SetRigid(Engine::RigidBodyComponent* rigid) { rigidBodyComponent = rigid; };

	// データ構造体取得
	Data& GetData() { return data_; }

	// 反応ボタン設定
	void SetGamePadButton(GamePadButton pad) { button_.SetGamePadButton(pad); };

	// 方向取得
	Vector3 GetDirection() const { return direction_; }
	// 方向指定
	void SetDirection(const Vector3& dire) { direction_ = dire; }

private:
	bool isMove_ = true;					// 移動出来るか
	Data data_;
	ComboButton button_ = ComboButton(GamePadButton::GAMEPAD_B, ComboButtonInputType::kPressed); // コンボボタン

private:
	Vector3 direction_ = {};
private: // 貰いもの 
	Engine::AnimationComponent* animationComponent = nullptr;	// アニメーション
	MovementComponent* moveComponent = nullptr;			// 移動
	Engine::WorldTransform* worldTransform = nullptr;			// ワールドトランスフォーム
	Engine::RigidBodyComponent* rigidBodyComponent = nullptr;	// リジットボディー
};

