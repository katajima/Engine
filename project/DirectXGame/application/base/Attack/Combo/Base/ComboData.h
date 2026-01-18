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

	ComboCondition comboCondition{};		// コンボ条件クラス
	ComboMotion motion{};					// コンボ用モーションクラス
	ComboCamera camera{};					// コンボ用カメラクラス
	ComboHitBox hitBox{};					// コンボ用ヒットボックスクラス
	ComboEffect effect{};					// コンボ用エフェクト
private:
	float timer_ = 0.0f;					// 時間
};





