#pragma once
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
// 前方宣言
namespace Engine {
	class AnimationComponent;	// アニメーション
	class RigidBodyComponent;	// リジットボディー
}

class MovementComponent;	// 移動関係
class BaseCharacter;		// キャラクター
class BaseWeapon;			// 武器
class JumpSystem;			// ジャンプシステム
class LockOnSystem;			// ロックオンシステム

namespace Combo {

	class ComboMove {
	public:
		// データ構造体
		struct Data
		{
			float moveWindowStart_ = 0.1f;			// 移動受付スタート
			float moveWindowEnd_ = 0.5f;			// 移動受付エンド
			float speed_ = 0.0f;					// 移動速度

			// 強制的に移動
			bool isCompulsionMove_ = true;			

			// 空中でのコンボで重力はあるか？
			bool isGravity_ = true;					
			// 重力スケール
			float gravityScale_ = 1.0f;				

			// 移動タイプ
			MoveType moveType = MoveType::kForward;	
		};

		// 開始
		void Enter(BaseCharacter* owner);

		// 更新
		void Update(const Engine::Input& input, float timer, float dt);

		// 終了
		void Exit(BaseCharacter* owner);

	public:
		// 移動できるか
		bool IsMove() const { return isMove_; }
		// 移動可能時間設定
		void MoveStartEnd(float start, float end) {
			data_.moveWindowStart_ = start;
			data_.moveWindowEnd_ = end;
		};
		// データ構造体取得
		Data& GetData() { return data_; }
		// 方向取得
		Vector3 GetDirection() const { return direction_; }
		// 方向指定
		void SetDirection(const Vector3& dire) { direction_ = dire; }
	private:
		// 移動タイプによる処理
		void MoveTypeProcess(const Engine::Input& input, float timer, float dt);
		// 重力処理
		void GravityProcess();
	private:
		// 移動
		MovementComponent* moveComponent = nullptr;
		// ワールドトランスフォーム
		Engine::WorldTransform* worldTransform = nullptr;
		// リジットボディー
		Engine::RigidBodyComponent* rigidBodyComponent = nullptr;
		// ロックオンシステム
		LockOnSystem* lockOnSystem = nullptr;
	private:
		// データ
		Data data_;
		// 方向
		Vector3 direction_ = {};
		// 移動出来るか
		bool isMove_ = true;
	};
}
