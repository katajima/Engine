#include "ComboData.h"

#pragma region ComboData

// 開始
void ComboData::Enter(BaseCharacter* owner) {
	// 時間初期化
	timer_ = 0.0f;
	// コンボ用モーションクラス開始
	motion.Enter(owner);
	// コンボ用条件クラス開始
	comboCondition.Enter(owner);
	// コンボ用カメラクラス開始
	camera.Enter();
	// コンボ用ヒットボックスクラス開始
	hitBox.Enter(owner);
	// コンボ用エフェクトクラス開始
	effect.Enter(owner);
}

// 更新
void ComboData::Update(const Engine::Input& input, float dt) {
	// 時間更新
	timer_ += dt;
	// コンボ用モーションクラス更新
	motion.Update(input, timer_, dt);
	// コンボ用条件クラス更新
	comboCondition.Update(input, timer_, dt);
	// コンボ用カメラクラス更新
	camera.Update(timer_, dt);
	// コンボ用ヒットボックスクラス更新
	hitBox.SetDirection(motion.GetDirection());
	hitBox.Update(input, timer_, dt);
	// コンボ用エフェクトクラス更新
	effect.Update(timer_, dt);
}

// 終了
void ComboData::Exit(BaseCharacter* owner) {
	// 時間リセット
	timer_ = 0.0f;
	// コンボ用モーションクラス終了
	motion.Exit(owner);
	// 条件クラス終了
	comboCondition.Exit();
	// コンボ用カメラクラス終了
	camera.Exit();
	// コンボ用ヒットボックスクラス終了
	hitBox.Exit();
	// コンボ用エフェクトクラス終了
	effect.Exit(owner);
}

#pragma endregion // コンボデータ