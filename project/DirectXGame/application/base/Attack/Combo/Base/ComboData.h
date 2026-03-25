#pragma once
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "DirectXGame/application/base/Attack/Combo/Receiver/ComboCondition.h"
#include "DirectXGame/application/base/Attack/Combo/Motion/ComboMotion.h"
#include "DirectXGame/application/base/Attack/Combo/HitBox/ComboHitBox.h"
#include "DirectXGame/application/base/Attack/Combo/Effect/ComboEffect.h"
#include "ComboSequencer.h"


namespace Combo {
	// コンボデータ
	class ComboData {
	public:

		// 開始
		void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx);

		// 更新
		void Update(const Character::CharacterContext& ctx);

		// 終了
		void Exit(Character::BaseCharacter* owner);
	public:
		// コンボ条件クラス取得
		ComboCondition& GetComboCondition() { return comboCondition; }
		// コンボモーションクラス取得
		ComboMotion& GetComboMotion() { return motion; }
		// コンボカメラクラス取得
		ComboCamera& GetComboCamera() { return camera; }
		// コンボヒットボックスクラス取得
		ComboHitBox& GetComboHitBox() { return hitBox; }
		// コンボエフェクトクラス取得
		ComboEffect& GetComboEffect() { return effect; }
		// コンボタイプ取得
		Type GetType() const { return type; }
	public:	// 時間
		// 経過時間取得
		float GetTimer() const { return timer_; }
		// 経過時間リセット
		void ResetTimer() { timer_ = 0.0f; }
		// 経過時間設定
		void SetTimer(float time) { timer_ = time; } 
		// 攻撃タイプ設定
		void SetType(Type type) { this->type = type; }
	private:
		ComboCondition comboCondition{};		// コンボ条件クラス
		ComboMotion motion{};					// コンボ用モーションクラス
		ComboCamera camera{};					// コンボ用カメラクラス
		ComboHitBox hitBox{};					// コンボ用ヒットボックスクラス
		ComboEffect effect{};					// コンボ用エフェクト
		float timer_ = 0.0f;					// 時間
		Type type = Type::kMelle;// 攻撃タイプ
	};
};




