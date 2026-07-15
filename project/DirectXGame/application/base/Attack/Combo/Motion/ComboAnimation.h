#pragma once
#include "DirectXGame/application/base/Attack/Combo/Input/ComboButton.h"
#include "DirectXGame/application/base/Attack/Combo/Base/ComboGlobalData.h"
// 前方宣言
namespace Engine {
	class AnimationComponent;	// アニメーション
	class RigidBodyComponent;	// リジットボディー
	class WorldTransform;		// Transformアニメーションの適用先
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
		void Update(const Character::CharacterContext& ctx,float timer, bool isDebug);

		// 終了
		void Exit(Character::BaseCharacter* owner);

	public:
		// データ構造体取得
		GlobalAnimation& GetData() { return data_; }
		// 終了条件設定
		void SetEndConditionType(EndConditionType type) { endType = type; }
	private:
		// 指定時間からTransformアニメーションの補間率を計算する
		float CalculateTransformAnimationRate(float timer) const;
		// 攻撃前移動アニメーションを使用できる設定か判定する
		bool CanUsePreMoveAnimation() const;
		// 攻撃アニメーションの再生時間をコンボ時間から計算する
		float CalculateAttackAnimationTime(float timer) const;
		// 指定名のスケルタルアニメーションを再生状態へ設定する
		void PlaySkeletalAnimation(const std::string& animationName, float blendTime, bool isLoop, float speed);
		// 攻撃前移動アニメーションから攻撃アニメーションへの切り替えを更新する
		bool UpdatePreMoveAnimation(float timer, bool isHitStop);
		// 攻撃アニメーションを開始し、必要なら再生時間を初期化する
		void StartAttackAnimation(float startTime);
		// 前フレームとの差分でTransformアニメーションを適用する
		void ApplyTransformAnimation(float timer);
		// 現在適用中のTransformオフセットを取り除く
		void RemoveTransformAnimationOffset();

	private:
		GlobalAnimation data_;
	private:
		// アニメーション
		Engine::AnimationComponent* animationComponent = nullptr;
		// 移動コンポーネント
		const MovementComponent* movementComponent = nullptr;
		// 終了条件
		EndConditionType endType = EndConditionType::kOnTimer;
		// 着地しているか
		bool onGlound = true;
		// アニメーションが存在しているか
		bool isAnimation = false;
		// Transformアニメーションの適用先
		Engine::WorldTransform* worldTransform_ = nullptr;
		// 前フレームまでに適用したTransformオフセット
		Transform appliedTransformOffset_{};
		// 攻撃前移動アニメーションを現在再生しているか
		bool isPlayingPreMoveAnimation_ = false;
		// 攻撃アニメーションをすでに開始したか
		bool hasStartedAttackAnimation_ = false;
		// 攻撃アニメーションの再生開始として扱うコンボ時間
		float attackAnimationStartTime_ = 0.0f;

	};




}
