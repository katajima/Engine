#pragma once
#include "DirectXGame/application/base/Attack/AttackData.h"
#include "ComboMotion.h"
#include "ComboHitBox.h"
#include "ComboEffect.h"
#include "ComboSequencer.h"


class BaseCharacter;		// キャラクター

// コンボデータ
class ComboData {
public:

	// 開始
	void Enter(BaseCharacter* owner);

	// 更新
	void Update(const Engine::Input& input, float dt);

	// 終了
	void Exit(BaseCharacter* owner);
public:
	// コンボ条件クラス
	ComboCondition& GetComboCondition() { return comboCondition; }
	// コンボモーションクラス
	ComboMotion& GetComboMotion() { return motion; }
	// コンボカメラクラス
	ComboCamera& GetComboCamera() { return camera; }
	// コンボヒットボックスクラス
	ComboHitBox& GetComboHitBox() { return hitBox; }
	// コンボエフェクトクラス
	ComboEffect& GetComboEffect() { return effect; }
public:	// 時間
	// 経過時間取得
	float GetTimer() const { return timer_; }
	// 経過時間リセット
	void ResetTimer() { timer_ = 0.0f; }
	// 経過時間設定
	void SetTimer(float time) { timer_ = time; }
private:
	ComboCondition comboCondition{};		// コンボ条件クラス
	ComboMotion motion{};					// コンボ用モーションクラス
	ComboCamera camera{};					// コンボ用カメラクラス
	ComboHitBox hitBox{};					// コンボ用ヒットボックスクラス
	ComboEffect effect{};					// コンボ用エフェクト
	float timer_ = 0.0f;					// 時間
};





