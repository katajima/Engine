#pragma once
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
// 前方宣言
namespace Engine {
	class AnimationComponent;	// アニメーション
	class RigidBodyComponent;	// リジットボディー
}


class MovementComponent;	// 移動関係
namespace Character {
	class BaseCharacter;		// キャラクター
}
class BaseWeapon;			// 武器
class JumpSystem;			// ジャンプシステム

namespace Combo {

	/// <summary>
	/// コンボアニメーション
	/// </summary>
	class ComboAnimation {
	public:
		// 開始
		void Enter(Character::BaseCharacter* owner);

		// 更新
		void Update(float timer, float dt, bool isDebug);

		// 終了
		void Exit(Character::BaseCharacter* owner);

	public:
		// データ構造体取得
		GlobalAnimation& GetData() { return data_; }
		// 終了条件設定
		void SetEndConditionType(EndConditionType type) { endType = type; }
	private:
		GlobalAnimation data_;
	private:
		// アニメーション
		Engine::AnimationComponent* animationComponent = nullptr;
		// 移動コンポーネント
		const MovementComponent* movementComponent = nullptr;
		// 終了条件
		EndConditionType endType = EndConditionType::kOnTimer;
		//
		bool onGlound = true;
	};




}