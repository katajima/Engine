#pragma once
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"

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
		struct Data {
			// アニメーション
			std::string animationName_ = "no";		// アニメーション名前
			bool animationLoop_ = false;			// アニメーションループ
			float animationSpeed_ = 1.0f;			// アニメーションスピード
			float animationBlendTime_ = 0.1f;		// アニメーションブレンド時間
		};


		// 開始
		void Enter(Character::BaseCharacter* owner);

		// 更新
		void Update(const Engine::Input& input, float timer, float dt);

		// 終了
		void Exit(Character::BaseCharacter* owner);

	public:
		// データ構造体取得
		Data& GetData() { return data_; }
	private:
		Data data_;
	private:
		// アニメーション
		Engine::AnimationComponent* animationComponent = nullptr;
	};




}