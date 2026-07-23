#pragma once
#include "ComboAnimation.h"
#include "ComboMove.h"


namespace Combo {
	/// <summary>
	/// コンボモーション
	/// </summary>
	class ComboMotion {
	public:
		/// <summary>
		/// 開始
		/// </summary>
		void Enter(Character::BaseCharacter* owner, const Character::CharacterContext& ctx);
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
		/// 終了条件設定
		/// </summary>
		void SetEndConditionType(EndConditionType type) { endType = type; }

		/// <summary>
		/// コンボ用アニメーションクラス取得
		/// </summary>
		ComboAnimation& GetComboAnimation() { return comboAnimation_; }
		/// <summary>
		/// コンボ用移動クラス取得
		/// </summary>
		ComboMove& GetComboMove() { return comboMove_; }
	private:
		// コンボ用アニメーションクラス
		ComboAnimation comboAnimation_;
		// コンボ用移動クラス
		ComboMove comboMove_;
		// 終了条件
		EndConditionType endType = EndConditionType::kOnTimer;
	};
}
