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
		/// <summary>
		/// 開始
		/// </summary>
		void Enter(Character::BaseCharacter* owner);

		/// <summary>
		/// 更新
		/// </summary>
		void Update(const Character::CharacterContext& ctx,float timer, bool isDebug);

		/// <summary>
		/// 終了
		/// </summary>
		void Exit(Character::BaseCharacter* owner);

	public:
		/// <summary>
		/// データ構造体取得
		/// </summary>
		GlobalAnimation& GetData() { return data_; }
		/// <summary>
		/// 終了条件設定
		/// </summary>
		void SetEndConditionType(EndConditionType type) { endType = type; }
	private:
		/// <summary>
		/// 指定時間からTransformアニメーションの補間率を計算する
		/// </summary>
		float CalculateTransformAnimationRate(float timer) const;
		/// <summary>
		/// 攻撃前移動アニメーションを使用できる設定か判定する
		/// </summary>
		bool CanUsePreMoveAnimation() const;
		/// <summary>
		/// 攻撃アニメーションの再生時間をコンボ時間から計算する
		/// </summary>
		float CalculateAttackAnimationTime(float timer) const;
		/// <summary>
		/// 指定名のスケルタルアニメーションを再生状態へ設定する
		/// </summary>
		void PlaySkeletalAnimation(const std::string& animationName, float blendTime, bool isLoop, float speed);
		/// <summary>
		/// 攻撃前移動アニメーションから攻撃アニメーションへの切り替えを更新する
		/// </summary>
		bool UpdatePreMoveAnimation(float timer, bool isHitStop);
		/// <summary>
		/// 攻撃アニメーションを開始し、必要なら再生時間を初期化する
		/// </summary>
		void StartAttackAnimation(float startTime);
		/// <summary>
		/// 前フレームとの差分でTransformアニメーションを適用する
		/// </summary>
		void ApplyTransformAnimation(float timer);
		/// <summary>
		/// 現在適用中のTransformオフセットを取り除く
		/// </summary>
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
