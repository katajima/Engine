#pragma once
#include "ComboAnimation.h"
#include "ComboMove.h"


namespace Combo {
	/// <summary>
	/// コンボモーション
	/// </summary>
	class ComboMotion {
	public:
		// 開始
		void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx);
		// 更新
		void Update(const Character::CharacterContext& ctx,float timer);
		// 終了
		void Exit(Character::BaseCharacter* owner);
	public:
		// コンボ用アニメーションクラス取得
		ComboAnimation& GetComboAnimation() { return comboAnimation_; }
		// コンボ用移動クラス取得
		ComboMove& GetComboMove() { return comboMove_; }
	private:
		// コンボ用アニメーションクラス
		ComboAnimation comboAnimation_;
		// コンボ用移動クラス
		ComboMove comboMove_;
	};
}
