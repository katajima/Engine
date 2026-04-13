#include "ComboData.h"


namespace Combo {

#pragma region ComboData

	// 開始
	void ComboData::Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx) {
		// 時間初期化
		timer_ = 0.0f;
		// コンボ用モーションクラス開始
		motion.Enter(owner, ctx);
		// コンボ用条件クラス開始
		comboCondition.Enter(owner);
		// コンボ用ヒットボックスクラス開始
		hitBox.Enter(owner, type);
		if (type == Type::kRange || type == Type::kMix) {
			// コンボ用遠距離クラス開始
			range.Enter(owner, ctx);
		}
		// コンボ用カメラクラス開始
		camera.Enter();
		// コンボ用エフェクトクラス開始
		effect.Enter(owner);
	}

	// 更新
	void ComboData::Update(const Character::CharacterContext& ctx) {
		// 時間更新
		if (!isDebug) {
			timer_ += ctx.dt;
		}
		// コンボ用条件クラス更新
		comboCondition.Update(ctx, timer_);
		// コンボ用モーションクラス更新
		motion.Update(ctx, timer_);
		// コンボ用ヒットボックスクラス更新
		hitBox.SetDirection(motion.GetComboMove().GetDirection());
		hitBox.Update(ctx, timer_);
		if (type == Type::kRange || type == Type::kMix) {
			// コンボ用遠距離クラス更新
			range.Update(ctx, timer_);
		}
		// コンボ用カメラクラス更新
		camera.Update(timer_, ctx.dt);
		// コンボ用エフェクトクラス更新
		effect.Update(timer_, ctx.dt);
	}

	// 終了
	void ComboData::Exit(Character::BaseCharacter* owner) {
		// 時間リセット
		timer_ = 0.0f;
		// 条件クラス終了
		comboCondition.Exit();
		// コンボ用モーションクラス終了
		motion.Exit(owner);
		// コンボ用ヒットボックスクラス終了
		hitBox.Exit();
		if (type == Type::kRange || type == Type::kMix) {
			// コンボ用遠距離クラス開始
			range.Exit(owner);
		}
		// コンボ用カメラクラス終了
		camera.Exit();
		// コンボ用エフェクトクラス終了
		effect.Exit(owner);
	}

#pragma endregion // コンボデータ

}